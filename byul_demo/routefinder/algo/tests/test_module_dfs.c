#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo.h"
#include <glib.h>
#include "internal/algo_utils.h"
#include "internal/dfs.h"

static void test_dfs_simple_route(void) {
    // map m = map_new();
    algo al = algo_new_default(10, 10, PATH_ALGO_DFS, TRUE);
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // route p = dfs_find_simple(m, start, goal);
    // route p = dfs_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_route(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    // map_free(m);
    algo_free(al);
}

static void test_dfs_blocked_route(void) {
    // map m = map_new();
    algo al = algo_new_default(10, 10, PATH_ALGO_DFS, TRUE);
    for (int y = 1; y < 10; y++) map_block_coord(al->m, 5, y);

    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // route p = dfs_find_simple(m, start, goal);
    // route p = dfs_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    // map_free(m);
    algo_free(al);
}

static void test_dfs_simple_route_8(void) {
    // map m = map_new();
    algo al = algo_new_default(10, 10, PATH_ALGO_DFS, TRUE);
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // route p = dfs_find_simple(m, start, goal);
    // route p = dfs_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    // map_free(m);
    algo_free(al);
}

static void test_dfs_blocked_route_8(void) {
    // map m = map_new();
    algo al = algo_new_default(10, 10, PATH_ALGO_DFS, TRUE);
    for (int y = 1; y < 10; y++) map_block_coord(al->m, 3, y);

    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // route p = dfs_find_simple(m, start, goal);
    // route p = dfs_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    // map_free(m);
    algo_free(al);
}


int run_dfs_tests(void) {
    g_test_add_func("/dfs/simple_route", test_dfs_simple_route);
    g_test_add_func("/dfs/blocked_route", test_dfs_blocked_route);

    g_test_add_func("/dfs/simple_route_8", test_dfs_simple_route_8);
    g_test_add_func("/dfs/blocked_route_8", test_dfs_blocked_route_8);    
    return 0;
}
