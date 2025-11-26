/*
** EPITECH PROJECT, 2025
** mirror_rtype
** File description:
** test_debug
*/

#include <gtest/gtest.h>

// ############################################################################
// ############### THIS FILE IS FOR DEBUGGING YOUR TESTS FAILED ###############
// ##################### PUT DEBUGGING PRINTS TO HELP YOU #####################
// ######################## THERE IS AN EXAMPLE BELOW #########################
// ############################################################################

// When I made my tests to kill an entity with my EntityManager, I had SegV
// Then i did another test with debugging prints, do the same if you have
// tests failed :)


// #include "ECS/Entity/EntityManager.hpp"
// #include <iostream>

// struct Position {
//     float x, y;
//     Position(float xx, float yy) : x(xx), y(yy) {}
// };

// TEST(DebugTest, KillEntitySimple) {
//     EntityManager em;

//     std::cout << "Registering component..." << std::endl;
//     em.register_component<Position>();

//     std::cout << "Spawning entity..." << std::endl;
//     Entity e = em.spawn_entity("TestEntity");

//     std::cout << "Entity ID: " << static_cast<size_t>(e) << std::endl;
//     std::cout << "Is alive before kill: " << em.is_alive(e) << std::endl;

//     std::cout << "Killing entity..." << std::endl;
//     em.kill_entity(e);  // <- Le SEGFAULT arrive probablement ici

//     std::cout << "Is alive after kill: " << em.is_alive(e) << std::endl;
//     EXPECT_FALSE(em.is_alive(e));
// }