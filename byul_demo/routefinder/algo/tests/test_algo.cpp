#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

// extern "C" {
    #include "internal/map.h"
    #include "internal/coord.h"
    #include "internal/route.h"
    #include "internal/algo.h"
    #include "internal/algo_utils.h"
// }

#include <iostream>

TEST_CASE("BFS: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_bfs(m, start, goal, 100, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("BFS: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_bfs(m, start, goal, 100, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_route(m, p, 5);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("BFS: blocked route force failed") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_bfs(m, start, goal, 25, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_route(m, p, 5);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("DFS: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_dfs(m, start, goal, 100, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("DFS: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_dfs(m, start, goal, 100, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("DFS: blocked route force failed") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_dfs(m, start, goal, 20, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("dijkstra: simple route") {
    map_t* m = map_new();
    // map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_dijkstra(m, start, goal, default_cost, 1000, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("dijkstra: blocked route") {
    map_t* m = map_new();
    // map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_dijkstra(m, start, goal, default_cost, 1000, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("dijkstra: blocked route force failed") {
    map_t* m = map_new();
    // map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_dijkstra(m, start, goal, default_cost, 20, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("astar: simple route") {
    map_t* m = map_new();
    // map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_astar(m, start, goal, default_cost, default_heuristic, 
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("astar: blocked route") {
    map_t* m = map_new();
    // map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_astar(m, start, goal, default_cost, default_heuristic, 
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("astar: blocked route force failed ") {
    map_t* m = map_new();
    // map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_astar(m, start, goal, default_cost, default_heuristic, 
        50, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("fast_marching: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_fast_marching(m, start, goal, 
        default_cost, 1000, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("fast_marching: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_fast_marching(m, start, goal, 
        default_cost, 1200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("fast_marching: blocked route force failed ") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_fast_marching(m, start, goal, 
        default_cost, 50, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("greedy_best_first: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_greedy_best_first(m, start, goal, 
        default_heuristic, 1000, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("greedy_best_first: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_greedy_best_first(m, start, goal, 
        default_heuristic, 1200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("greedy_best_first: blocked route force failed ") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_greedy_best_first(m, start, goal, 
        default_heuristic, 25, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("ida_star: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_ida_star(m, start, goal, default_cost, nullptr, 
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("ida_star: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_ida_star(m, start, goal, default_cost, nullptr, 
        2000, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("ida_star: blocked route force failed ") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_ida_star(m, start, goal, default_cost, nullptr, 
        50, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("fringe_search: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_fringe_search(m, start, goal, 
        default_cost, default_heuristic, 1.0,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("fringe_search: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_fringe_search(m, start, goal, 
        default_cost, 
        default_heuristic, 1.0,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("fringe_search: blocked route force failed ") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_fringe_search(m, start, goal, 
        default_cost, 
        default_heuristic, 1.0, 
        20, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("weighted_astar: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_weighted_astar(m, start, goal, 
        default_cost, default_heuristic, 1.0,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("weighted_astar: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_weighted_astar(m, start, goal, 
        default_cost, 
        default_heuristic, 1.0,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("weighted_astar: blocked route force failed ") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_weighted_astar(m, start, goal, 
        default_cost, 
        default_heuristic, 1.0, 
        20, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("rta_star: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_rta_star(m, start, goal, 
        default_cost, default_heuristic, 9,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("rta_star: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_rta_star(m, start, goal, 
        default_cost, 
        default_heuristic, 9,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("rta_star: blocked route force failed ") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_rta_star(m, start, goal, 
        default_cost, 
        default_heuristic, 9, 
        10, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("sma_star: simple route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = find_sma_star(m, start, goal, 
        default_cost, default_heuristic, 1000,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("sma_star: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_sma_star(m, start, goal, 
        default_cost, 
        default_heuristic, 1000,
        200, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("sma_star: blocked route force failed ") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    route_t* p = find_sma_star(m, start, goal, 
        default_cost, 
        default_heuristic, 1000, 
        10, true);

    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == false);

    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
}

TEST_CASE("algo_all: blocked route") {
    // map_t* m = map_new();
    map_t* m = map_new_full(10, 10, MAP_NEIGHBOR_8);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    REQUIRE_FALSE(coord_equal(start, goal));

    // 장애물 삽입 (세로 차단)
    for (int y = 1; y < 10; ++y)
        map_block_coord(m, 5, y);

    algo_t* a = algo_new(m);
    algo_set_goal(a, goal);
    algo_set_start(a, start);
    algo_set_visited_logging(a, true);
    route_t* p = nullptr;

    // #include "internal/astar.h"
    std::cout << "astar.h\n";
    algo_set_type(a, ROUTE_ALGO_ASTAR);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/bfs.h"
    std::cout << "bfs.h\n";
    algo_set_type(a, ROUTE_ALGO_BFS);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/dfs.h"
    std::cout << "dfs.h\n";
    algo_set_type(a, ROUTE_ALGO_DFS);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/dijkstra.h"
    std::cout << "dijkstra.h\n";
    algo_set_type(a, ROUTE_ALGO_DIJKSTRA);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/fast_marching.h"
    std::cout << "fast_marching.h\n";
    algo_set_type(a, ROUTE_ALGO_FAST_MARCHING);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/fringe_search.h"
    std::cout << "fringe_search.h\n";
    algo_set_type(a, ROUTE_ALGO_FRINGE_SEARCH);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/greedy_best_first.h"
    std::cout << "greedy_best_first.h\n";
    algo_set_type(a, ROUTE_ALGO_GREEDY_BEST_FIRST);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/ida_star.h"
    std::cout << "ida_star.h\n";
    algo_set_type(a, ROUTE_ALGO_IDA_STAR);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/rta_star.h"
    std::cout << "rta_star.h\n";
    algo_set_type(a, ROUTE_ALGO_RTA_STAR);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/sma_star.h"
    std::cout << "sma_star.h\n";
    algo_set_type(a, ROUTE_ALGO_SMA_STAR);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);

    // #include "internal/weighted_astar.h"        
    std::cout << "weighted_astar.h\n";
    algo_set_type(a, ROUTE_ALGO_WEIGHTED_ASTAR);
    p = algo_find(a);
    REQUIRE(p != nullptr);
    CHECK(route_get_success(p) == true);
    route_print(p);
    map_print_ascii_with_visited_count(m, p, 5);    

    route_free(p);
    coord_free(start);
    coord_free(goal);
    map_free(m);
    algo_free(a);
}