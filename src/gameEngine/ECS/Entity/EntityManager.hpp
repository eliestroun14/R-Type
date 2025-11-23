/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** EntityManager
*/

#ifndef ENTITYMANAGER_HPP_
#define ENTITYMANAGER_HPP_

#include "../Component/ComponentManager.hpp"
#include "Entity.hpp"
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <functional>
#include <any>
#include <unordered_set>
#include <stdexcept>
#include <optional>

class EntityManager {
    public :
        EntityManager() = default;

        // --- Enregistrement d’un type de composant ---
        template<class Component>
        ComponentManager<Component>& register_component() {
            std::type_index key(typeid(Component)); // créer une clé unique pour le type Component

            // try_emplace: n'insère que si la clé n'existe pas et renvoie pair<iterator,bool>
            auto [it, inserted] = this->_components_arrays.try_emplace(
                key,
                std::any(ComponentManager<Component>{})
            );

            // Si on vient d'insérer, on doit aussi enregistrer l'eraser correspondant.
            if (inserted) {
                _erasers[key] = [](EntityManager& r, Entity const& e) {
                    r.get_components<Component>().erase(static_cast<size_t>(e));
                };
            }

            // it->second est la std::any associée; on fait un any_cast vers notre type.
            // any_cast en référence est sûr ici, car nous venons soit d'insérer, soit la
            // map contenait déjà la même type d'objet.
            return std::any_cast<ComponentManager<Component>&>(it->second);
        }


        // --- Récupération d’un tableau de composants ---
        template <class Component>
        ComponentManager<Component> &get_components() {
            return std::any_cast<ComponentManager<Component>&>(
                this->_components_arrays.at(std::type_index(typeid(Component)))
            );
        }

        // register_compoenent doit être absolument appelé avec le component en question
        template <class Component>
        ComponentManager<Component> const &get_components() const {
            return std::any_cast<const ComponentManager<Component>&>(
                this->_components_arrays.at(std::type_index(typeid(Component)))
            );
        }


        template<class Component>
        std::optional<Component>& get_component(Entity const& e) {
            return get_components<Component>()[static_cast<std::size_t>(e)];
        }

        template<class Component>
        const std::optional<Component>& get_component(Entity const& e) const {
            return get_components<Component>()[static_cast<std::size_t>(e)];
        }

        // Usage de get_component()
        /*
            auto& pos_opt = EntityManager.get_component<Position>(entity);
            if (pos_opt) {
                Position& pos = *pos_opt;
                // utiliser pos
            }
        */


        Entity spawn_entity(std::string name) {
            std::size_t id;

            if (!this->_free_ids.empty()) {
                id = this->_free_ids.back();
                this->_free_ids.pop_back();
            } else
                id = this->_next_id++;

            this->_alive_entities.insert(id);
            return Entity(id, name);
        }


        void kill_entity(Entity const &entity) {
            std::size_t id = static_cast<std::size_t>(entity);

            if (this->_alive_entities.find(id) == this->_alive_entities.end())
                return; // l'entity est déjà morte

            for (auto& [key, fn] : this->_erasers)
                fn(*this, entity);

            this->_alive_entities.erase(id);
            this->_free_ids.push_back(id);
        }


        bool is_alive(Entity const& e) const {
            return this->_alive_entities.find(static_cast<std::size_t>(e)) != this->_alive_entities.end();
        }


        template <class Component>
        bool has_component(Entity const& e) const {
            auto& components = get_components<Component>();

            std::size_t id = static_cast<std::size_t>(e);
            return id < components.size() && components[id].has_value();
        }


        template <class Component>
        typename ComponentManager<Component>::reference_type add_component(Entity const &e, Component &&c)
        {
            if (!is_alive(e))
                throw std::runtime_error("Cannot add component to dead entity")
            return get_components<Component>().insert_at(static_cast<std::size_t>(e), std::forward<Component>(c));
        }


        template<class Component, class... Params>
        typename ComponentManager<Component>::reference_type emplace_component(Entity const& e, Params&&... ps) {
            if (!is_alive(e))
                throw std::runtime_error("Cannot add component to dead entity")
            return get_components<Component>().emplace_at(static_cast<std::size_t>(e), std::forward<Params>(ps)...);
        }


        template<class Component>
        void remove_component(Entity const& e) {
            if (!is_alive(e))
                throw std::runtime_error("Cannot add component to dead entity")
            get_components<Component>().erase(static_cast<std::size_t>(e));
        }


    private:
        // But : associer un type de composant → conteneur ComponentManager approprié.
        // Clé : std::type_index.
        // Valeur : std::any contenant un ComponentManager<Component> concret.
        std::unordered_map<std::type_index, std::any> _components_arrays;

        // But : pour chaque type enregistré, stocker une fonction capable de supprimer un composant d’une entité donnée.
        // Structure :
        // clé : type_index
        // valeur : lambda void(EntityManager&, Entity)
        // Usage : kill_entity(e) parcourt tous les effaceurs, donc supprime tous les composants de tous les types associés à e.
        std::unordered_map<std::type_index, std::function<void(EntityManager&,Entity const&)>> _erasers;

        size_t _next_id = 0;
        std::vector<std::size_t> _free_ids;

        std::unordered_set<std::size_t> _alive_entities;
};

#endif /* !ENTITYMANAGER_HPP_ */
