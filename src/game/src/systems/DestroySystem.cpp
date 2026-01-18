/*
** EPITECH PROJECT, 2026
** mirror_rtype
** File description:
** DestroySystem
*/

#include <common/logger/Logger.hpp>
#include <common/error/Error.hpp>
#include <game/systems/DestroySystem.hpp>

constexpr float DESTROY_MARGIN_X = 2000.0f;
constexpr float DESTROY_MARGIN_Y = 2000.0f;

void DestroySystem::onUpdate(float dt)
{
    try {
        auto& transforms = this->_engine.getComponents<Transform>();
        std::vector<size_t> entitiesToDestroy;
        
        // Calculer les limites réelles de destruction
        // À gauche : -1000 (1000px avant le bord gauche à x=0)
        // À droite : 1920 + 1000 = 2920
        // En haut : -500 (500px avant le bord haut à y=0)
        // En bas : 1080 + 500 = 1580
        const float minX = -DESTROY_MARGIN_X;
        const float maxX = WINDOW_WIDTH + DESTROY_MARGIN_X;
        const float minY = -DESTROY_MARGIN_Y;
        const float maxY = WINDOW_HEIGHT + DESTROY_MARGIN_Y;
        
        LOG_DEBUG_CAT("DestroySystem", "onUpdate: checking {} entities", this->_entities.size());
        LOG_DEBUG_CAT("DestroySystem", "Destroy bounds: X[{}, {}] Y[{}, {}]", minX, maxX, minY, maxY);
        
        for (size_t e : this->_entities) {
            if (!transforms[e].has_value()) {
                continue;
            }
            
            auto& transform = transforms[e].value();
            
            // Vérifier si l'entité dépasse les limites
            bool outOfBounds = false;
            
            // Vérifier X : en-dessous de -1000 ou au-delà de 2920
            if (transform.x < minX || transform.x > maxX) {
                outOfBounds = true;
                LOG_DEBUG_CAT("DestroySystem", "Entity {} out of X bounds: x={} (limits: [{}, {}])", 
                                e, transform.x, minX, maxX);
            }
            
            // Vérifier Y : en-dessous de -500 ou au-delà de 1580
            if (transform.y < minY || transform.y > maxY) {
                outOfBounds = true;
                LOG_DEBUG_CAT("DestroySystem", "Entity {} out of Y bounds: y={} (limits: [{}, {}])", 
                                e, transform.y, minY, maxY);
            }
            
            // Marquer pour destruction si hors limites
            if (outOfBounds) {
                entitiesToDestroy.push_back(e);
                LOG_INFO_CAT("DestroySystem", "Marking entity {} for destruction (x={}, y={})", 
                                e, transform.x, transform.y);
            }
        }
        
        // Détruire toutes les entités marquées
        for (size_t e : entitiesToDestroy) {
            this->_engine.destroyEntity(e);
            LOG_INFO_CAT("DestroySystem", "Entity {} destroyed", e);
        }
        
        if (!entitiesToDestroy.empty()) {
            LOG_DEBUG_CAT("DestroySystem", "Destroyed {} entities this frame", entitiesToDestroy.size());
        }
        
    } catch (const Error& e) {
        LOG_ERROR_CAT("DestroySystem", "Error in DestroySystem::onUpdate: {}", e.what());
        throw;
    } catch (const std::exception& e) {
        LOG_ERROR_CAT("DestroySystem", "Unexpected error in DestroySystem::onUpdate: {}", e.what());
        throw Error(ErrorType::GameplayError, "DestroySystem update failed: " + std::string(e.what()));
    }
}
