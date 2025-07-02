#include "internal/astar.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo.h"
#include <glib.h>
#include "internal/algo_utils.h"

static void test_astar_simple_route(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    algo al = algo_new_full(10, 10, MAP_NEIGHBOR_8, PATH_ALGO_ASTAR,
        default_cost,
        default_heuristic,
        NULL,
        NULL,
        TRUE    
    );

    // route p = astar_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_route(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

static void test_astar_blocked_route(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    algo al = algo_new_full(10, 10, MAP_NEIGHBOR_8, PATH_ALGO_ASTAR,
        default_cost,
        default_heuristic,
        NULL,
        NULL,
        TRUE    
    );
    for (int y = 1; y < 10; y++) map_block_coord(al->m, 5, y);    

    // route p = astar_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

int run_astar_tests(void) {
    g_test_add_func("/astar/simple_route", test_astar_simple_route);
    g_test_add_func("/astar/blocked_route", test_astar_blocked_route);
    return 0;
}
