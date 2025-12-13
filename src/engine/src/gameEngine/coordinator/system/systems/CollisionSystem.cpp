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

                if (e2 < healths.size() && healths[e2]) {
                    auto& health2 = healths[e2].value();
                    health2.currentHealth -= 10;
                    std::cout << "Entity " << e2
                              << " took damage! HP: " << health2.currentHealth
                              << "/" << health2.maxHp << std::endl;

                    if (health2.currentHealth <= 0) {
                        std::cout << "Entity " << e2
                                  << " is destroyed!" << std::endl;
                        Entity entity2 = Entity::fromId(e2);
                        this->_coordinator.removeComponent<Transform>(entity2);
                        this->_coordinator.removeComponent<Sprite>(entity2);
                        this->_coordinator.removeComponent<Health>(entity2);
                    }
                }
            }
        }
    }
}
