#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo_utils.h"
#include "internal/rta_star.h"
#include <glib.h>
#include <stdio.h>

static void test_rta_star_basic(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // 깊이 제한 5
    rta_star_config cfg = rta_star_config_new_full(5); 

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_RTA_STAR,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
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
    rta_star_config_free(cfg);
}

static void test_rta_star_blocked(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // 깊이 제한 7 
    rta_star_config cfg = rta_star_config_new_full(7); 

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_RTA_STAR,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    // 중앙 벽
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    rta_star_config_free(cfg);
}

static void test_rta_star_depth6(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    rta_star_config cfg = rta_star_config_new_full(6); 

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_RTA_STAR,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    // 중앙 벽
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    rta_star_config_free(cfg);
}

static void test_rta_star_depth3(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    rta_star_config cfg = rta_star_config_new_full(3); 

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_RTA_STAR,
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    // 중앙 벽
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_false(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    rta_star_config_free(cfg);
}

int run_rta_star_tests(void) {
    g_test_add_func("/rta_star/basic", test_rta_star_basic);
    g_test_add_func("/rta_star/blocked", test_rta_star_blocked);
    g_test_add_func("/rta_star/blocked_depth6", test_rta_star_depth6);
    g_test_add_func("/rta_star/blocked_depth3", test_rta_star_depth3);

    return 0;
}
