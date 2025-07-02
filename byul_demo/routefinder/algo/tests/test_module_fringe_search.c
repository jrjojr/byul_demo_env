#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/fringe_search.h"
#include <glib.h>

static void test_fringe_search_basic(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FRINGE_SEARCH,
        default_cost,
        default_heuristic,
        NULL,
        NULL,
        TRUE
    );

    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_route(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

static void test_fringe_search_blocked(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    fringe_search_config cfg = fringe_search_config_new_full(3.0f);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FRINGE_SEARCH,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    // 중앙 수직 장애물
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));  // 우회 가능성 확인

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    fringe_search_config_free(cfg);
}

static void test_fringe_search_unblocked_y8(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FRINGE_SEARCH,
        default_cost,
        default_heuristic,
        NULL,
        NULL,
        TRUE
    );

    // 중앙 수직 장애물
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    map_unblock_coord(al->m, 5, 8);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));  // 우회 가능성 확인

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

static void test_fringe_search_unblocked_y9(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FRINGE_SEARCH,
        default_cost,
        default_heuristic,
        NULL,
        NULL,
        TRUE
    );

    // 중앙 수직 장애물
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    map_unblock_coord(al->m, 5, 9);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));  // 우회 가능성 확인

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

static void test_fringe_search_unblocked_y2(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FRINGE_SEARCH,
        default_cost,
        default_heuristic,
        NULL,
        NULL,
        TRUE
    );

    // 중앙 수직 장애물
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    map_unblock_coord(al->m, 5, 2);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));  // 우회 가능성 확인

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

static void test_fringe_search_unblocked_y1(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    fringe_search_config cfg = fringe_search_config_new_full(10.0f);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FRINGE_SEARCH,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    // 중앙 수직 장애물
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    map_unblock_coord(al->m, 5, 1);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));  // 우회 가능성 확인

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
fringe_search_config_free(cfg);    
}

int run_fringe_search_tests(void) {
    g_test_add_func("/fringe_search/basic",   test_fringe_search_basic);
    g_test_add_func("/fringe_search/blocked", test_fringe_search_blocked);
    g_test_add_func("/fringe_search/unblocked_y8", test_fringe_search_unblocked_y8);
    g_test_add_func("/fringe_search/unblocked_y9", test_fringe_search_unblocked_y9);
    g_test_add_func("/fringe_search/unblocked_y2", test_fringe_search_unblocked_y2);
    g_test_add_func("/fringe_search/unblocked_y1", test_fringe_search_unblocked_y1);
    return 0;
}
