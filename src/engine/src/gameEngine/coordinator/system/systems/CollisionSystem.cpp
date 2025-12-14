/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** CollisionSystem
*/

#include <engine/gameEngine/coordinator/ecs/system/systems/CollisionSystem.hpp>
#include <engine/gameEngine/coordinator/ecs/component/Components.hpp>
#include <engine/gameEngine/coordinator/ecs/entity/Entity.hpp>
#include <iostream>

bool CollisionSystem::checkAABBCollision(const Sprite& s1, const Sprite& s2)
{
    return s1.globalBounds.intersects(s2.globalBounds);
}

void CollisionSystem::onUpdate(float dt)
{
    auto& transforms = this->_coordinator.getComponents<Transform>();
    auto& sprites = this->_coordinator.getComponents<Sprite>();
    auto& healths = this->_coordinator.getComponents<Health>();
    auto& hitboxes = this->_coordinator.getComponents<HitBox>();
    auto& projectiles = this->_coordinator.getComponents<Projectile>();
    auto& playables = this->_coordinator.getComponents<Playable>();

    std::vector<size_t> entities;
    entities.reserve(this->_entities.size());
    for (size_t entity : this->_entities) {
        entities.push_back(entity);
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

            if (checkAABBCollision(s1, s2)) {
                // For debugging, print collision info
                std::cout << "Collision detected between Entity " << e1
                << " and Entity " << e2 << std::endl;

                // Apply damage to both entities if they have Health component
                if (e1 < healths.size() && healths[e1]) {
                    auto& health1 = healths[e1].value();
                    health1.currentHealth -= 10;
                    std::cout << "Entity " << e1
                              << " took damage! HP: " << health1.currentHealth
                              << "/" << health1.maxHp << std::endl;

                    if (health1.currentHealth <= 0) {
                        std::cout << "Entity " << e1
                                  << " is destroyed!" << std::endl;
                        Entity entity1 = Entity::fromId(e1);
                        this->_coordinator.removeComponent<Transform>(entity1);
                        this->_coordinator.removeComponent<Sprite>(entity1);
                        this->_coordinator.removeComponent<Health>(entity1);
                    }
                }

            // Projectile-friendly-fire rules:
            // - Player bullets must not damage playables (including self)
            // - Enemy bullets should only damage playables
            bool e1Projectile = e1 < projectiles.size() && projectiles[e1].has_value();
            bool e2Projectile = e2 < projectiles.size() && projectiles[e2].has_value();

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
                    this->_coordinator.removeComponent<Transform>(ent);
                    this->_coordinator.removeComponent<Sprite>(ent);
                    this->_coordinator.removeComponent<Health>(ent);
                    this->_coordinator.removeComponent<HitBox>(ent);
                }
            };

            auto destroyProjectile = [&](size_t projId) {
                Entity ent = Entity::fromId(projId);
                this->_coordinator.removeComponent<Transform>(ent);
                this->_coordinator.removeComponent<Sprite>(ent);
                this->_coordinator.removeComponent<HitBox>(ent);
                this->_coordinator.removeComponent<Projectile>(ent);
            };

            auto canHit = [&](const Projectile& proj, size_t targetId) {
                // Prevent hitting shooter
                if (targetId == static_cast<size_t>(proj.shooterId))
                    return false;
                bool targetPlayable = targetId < playables.size() && playables[targetId].has_value();
                if (proj.isFromPlayable) {
                    // Player bullets do not hit playables
                    return !targetPlayable;
                }
                // Enemy bullets only hit playables
                return targetPlayable;
            };

            // Handle projectile collisions
            if (e1Projectile && !e2Projectile) {
                const auto& proj = projectiles[e1].value();
                if (!canHit(proj, e2)) {
                    // Ignore collision with shooter or allies; keep projectile alive
                    continue;
                }
                applyDamage(e2, proj.damage);
                destroyProjectile(e1);
                continue;
            }
            if (e2Projectile && !e1Projectile) {
                const auto& proj = projectiles[e2].value();
                if (!canHit(proj, e1)) {
                    // Ignore collision with shooter or allies; keep projectile alive
                    continue;
                }
                applyDamage(e1, proj.damage);
                destroyProjectile(e2);
                continue;
            }

            // Non-projectile collision fallback: apply symmetric damage
            if (e1 < healths.size() && healths[e1]) {
                applyDamage(e1, 10);
            }
            if (e2 < healths.size() && healths[e2]) {
                applyDamage(e2, 10);
            }
        }
    }
}
