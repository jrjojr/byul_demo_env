#include "internal/dijkstra.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo.h"
#include "internal/algo_utils.h"
#include <glib.h>

static void test_map_print(void) {
    map m = map_new();
    map_block_coord(m, 1, 3);
    map_block_coord(m, 2, 3);
    map_block_coord(m, 3, 3);
    map_block_coord(m, 4, 3);
    map_block_coord(m, 5, 3);
    map_block_coord(m, 6, 3);
    map_block_coord(m, 7, 3);
    map_block_coord(m, 8, 3);
    map_block_coord(m, 9, 3);

    g_print("[TEST] map_print_ascii\n");
    map_print_ascii(m);
    map_free(m);
}

static void test_map_print_with_route(void) {
    // map m = map_new();
    algo al = algo_new();
    map_block_coord(al->m, 1, 3);
    map_block_coord(al->m, 2, 3);
    map_block_coord(al->m, 3, 3);
    map_block_coord(al->m, 4, 3);
    map_block_coord(al->m, 5, 3);
    map_block_coord(al->m, 6, 3);
    map_block_coord(al->m, 7, 3);
    map_block_coord(al->m, 8, 3);
    map_block_coord(al->m, 9, 3);

    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // route p = bfs_find_simple(m, start, goal);
    // route p = bfs_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_print("[TEST] map_print_ascii_with_route\n");
    map_print_ascii_with_route(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    // map_free(m);
    algo_free(al);
}

static void test_map_print_with_visited_count(void) {
    // map m = map_new();
    algo al = algo_new_default(10, 10, PATH_ALGO_BFS, TRUE);
    map_block_coord(al->m, 1, 3);
    map_block_coord(al->m, 2, 3);
    map_block_coord(al->m, 3, 3);
    map_block_coord(al->m, 4, 3);
    map_block_coord(al->m, 5, 3);
    map_block_coord(al->m, 6, 3);
    map_block_coord(al->m, 7, 3);
    map_block_coord(al->m, 8, 3);
    map_block_coord(al->m, 9, 3);

    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // route p = bfs_find_simple(m, start, goal);
    // route p = bfs_find(al, start, goal);
    route p = algo_find(al, start, goal);
    
    g_print("[TEST] map_print_ascii_with_visited_count\n");
    map_print_ascii_with_visited_count(al->m, p, start, goal);
    

    route_free(p);
    coord_free(start);
    coord_free(goal);
    // map_free(m);
    algo_free(al);
}

int run_map_print_tests(void) {
    g_test_add_func("/algo/map_print", test_map_print);
    g_test_add_func("/algo/map_print_with_route", test_map_print_with_route);
    g_test_add_func("/algo/map_print_with_visited_count", test_map_print_with_visited_count);
    return 0;
}
