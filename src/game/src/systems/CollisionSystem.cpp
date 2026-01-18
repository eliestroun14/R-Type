/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** CollisionSystem
*/

#include <game/systems/CollisionSystem.hpp>
#include <engine/ecs/component/Components.hpp>
#include <engine/ecs/entity/Entity.hpp>
#include <iostream>

bool CollisionSystem::checkAABBCollision(const Sprite& s1, const Sprite& s2)
{
    // If globalBounds are not set (e.g., on server), we can't check collision
    if (s1.globalBounds.width == 0 || s1.globalBounds.height == 0 ||
        s2.globalBounds.width == 0 || s2.globalBounds.height == 0) {
        return false;
    }
    return s1.globalBounds.intersects(s2.globalBounds);
}

void CollisionSystem::updateGlobalBounds(Sprite& sprite, const Transform& transform)
{
    // Manually calculate globalBounds from sprite rect and transform
    // This is needed on the server where RenderSystem doesn't run
    float width = sprite.rect.width * transform.scale;
    float height = sprite.rect.height * transform.scale;
    sprite.globalBounds = sf::FloatRect(transform.x, transform.y, width, height);
}

void CollisionSystem::onUpdate(float dt)
{
    auto& transforms = this->_engine.getComponents<Transform>();
    auto& sprites = this->_engine.getComponents<Sprite>();
    auto& healths = this->_engine.getComponents<Health>();
    auto& hitboxes = this->_engine.getComponents<HitBox>();
    auto& projectiles = this->_engine.getComponents<Projectile>();
    auto& teams = this->_engine.getComponents<Team>();

    std::vector<size_t> entities;
    entities.reserve(this->_entities.size());
    for (size_t entity : this->_entities) {
        entities.push_back(entity);
    }

    // Update globalBounds for all entities (needed on server where RenderSystem doesn't run)
    for (size_t e : entities) {
        if (transforms[e] && sprites[e]) {
            auto& sprite = sprites[e].value();
            auto& transform = transforms[e].value();
            updateGlobalBounds(sprite, transform);
        }
    }

    for (size_t i = 0; i < entities.size(); ++i) {
        size_t e1 = entities[i];

        if (!transforms[e1] || !sprites[e1] || !hitboxes[e1])
            continue;

        auto& t1 = transforms[e1].value();
        auto& s1 = sprites[e1].value();

        for (size_t j = i + 1; j < entities.size(); ++j) {
            size_t e2 = entities[j];

            if (!transforms[e2] || !sprites[e2] || !hitboxes[e2])
                continue;

            auto& t2 = transforms[e2].value();
            auto& s2 = sprites[e2].value();

            if (!checkAABBCollision(s1, s2))
                continue;

            bool e1Projectile = e1 < projectiles.size() && projectiles[e1].has_value();
            bool e2Projectile = e2 < projectiles.size() && projectiles[e2].has_value();

            // Get team components (default to NEUTRAL if not present)
            Team e1Team = e1 < teams.size() && teams[e1] ? teams[e1].value() : Team(TeamType::NEUTRAL);
            Team e2Team = e2 < teams.size() && teams[e2] ? teams[e2].value() : Team(TeamType::NEUTRAL);

            auto applyDamage = [&](size_t target, int damage) {
                if (target >= healths.size() || !healths[target])
                    return;
                auto& h = healths[target].value();
                h.currentHealth -= damage;
                std::cout << "Entity " << target << " took damage! HP: "
                          << h.currentHealth << "/" << h.maxHp << std::endl;
                if (h.currentHealth <= 0) {
                    std::cout << "Entity " << target << " is destroyed!" << std::endl;
                    Entity ent = Entity::fromId(target);
                    this->_engine.removeComponent<Transform>(ent);
                    this->_engine.removeComponent<Sprite>(ent);
                    this->_engine.removeComponent<Health>(ent);
                    this->_engine.removeComponent<HitBox>(ent);
                    if (target < teams.size() && teams[target]) {
                        this->_engine.removeComponent<Team>(ent);
                    }
                }
            };

            auto destroyProjectile = [&](size_t projId) {
                Entity ent = Entity::fromId(projId);
                this->_engine.removeComponent<Transform>(ent);
                this->_engine.removeComponent<Sprite>(ent);
                this->_engine.removeComponent<HitBox>(ent);
                this->_engine.removeComponent<Projectile>(ent);
                if (projId < teams.size() && teams[projId]) {
                    this->_engine.removeComponent<Team>(ent);
                }
            };

            // Handle projectile collisions with team rules
            if (e1Projectile && !e2Projectile) {
                if (!Team::canCollide(e1Team, e2Team, true)) {
                    continue;
                }

                const auto& proj = projectiles[e1].value();
                if (e2 == static_cast<size_t>(proj.shooterId)) {
                    continue;
                }

                applyDamage(e2, proj.damage);
                destroyProjectile(e1);
                continue;
            }

            if (e2Projectile && !e1Projectile) {
                // Check if projectile can hit target based on teams
                if (!Team::canCollide(e2Team, e1Team, true)) {
                    continue;
                }

                const auto& proj = projectiles[e2].value();
                if (e1 == static_cast<size_t>(proj.shooterId)) {
                    continue;
                }

                applyDamage(e1, proj.damage);
                destroyProjectile(e2);
                continue;
            }

            // If one entity is OBSTACLE, no damage is applied at all
            bool e1IsObstacle = e1Team.hasTeam(TeamType::OBSTACLE);
            bool e2IsObstacle = e2Team.hasTeam(TeamType::OBSTACLE);

            if (!e1IsObstacle && !e2IsObstacle) {
                bool e1IsPlayer = e1Team.hasTeam(TeamType::PLAYER);
                bool e2IsPlayer = e2Team.hasTeam(TeamType::PLAYER);
                bool e1IsEnemy = e1Team.hasTeam(TeamType::ENEMY) || e1Team.hasTeam(TeamType::BOSS);
                bool e2IsEnemy = e2Team.hasTeam(TeamType::ENEMY) || e2Team.hasTeam(TeamType::BOSS);

                if ((e1IsPlayer && e2IsEnemy) || (e1IsEnemy && e2IsPlayer)) {
                    if (e1IsPlayer && e2 < healths.size() && healths[e2]) {
                        applyDamage(e2, 10);
                    }
                    if (e2IsPlayer && e1 < healths.size() && healths[e1]) {
                        applyDamage(e1, 10);
                    }
                }
            }
        }
    }
}
