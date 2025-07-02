#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo_utils.h"
#include "internal/weighted_astar.h"
#include <glib.h>
#include <stdio.h>

static void test_module_weighted_astar_basic(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    weighted_astar_config cfg = weighted_astar_config_new_full(2.0f);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_WEIGHTED_ASTAR, 
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
    );

    // route p = weighted_astar_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_route(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    weighted_astar_config_free(cfg);
}

static void test_module_weighted_astar_blocked(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    weighted_astar_config cfg = weighted_astar_config_new_full(2.0f);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_WEIGHTED_ASTAR, 
        default_cost,
        default_heuristic,
        NULL,
        cfg,
        TRUE
        // FALSE
    );
    for (int y = 1; y < 10; y++) map_block_coord(al->m, 5, y);

    // route p = weighted_astar_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    weighted_astar_config_free(cfg);
}

int run_weighted_astar_tests(void) {
    g_test_add_func("/weighted_astar/basic", 
        test_module_weighted_astar_basic);

    g_test_add_func("/weighted_astar/blocked", 
        test_module_weighted_astar_blocked);
    return 0;
}
