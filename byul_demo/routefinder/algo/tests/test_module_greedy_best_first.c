#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo_utils.h"
#include <glib.h>
#include <stdio.h>

static void test_greedy_best_first_basic(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_GREEDY_BEST_FIRST,
        NULL,                      // cost 함수 없음
        default_heuristic,         // h(x)만 사용
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

static void test_greedy_best_first_blocked(void) {
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_GREEDY_BEST_FIRST,
        NULL,
        default_heuristic,
        NULL,
        NULL,
        TRUE
    );

    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);  // 중앙을 차단

    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

int run_greedy_best_first_tests(void) {
    g_test_add_func("/greedy_best_first/basic", test_greedy_best_first_basic);
    
    g_test_add_func("/greedy_best_first/blocked", 
        test_greedy_best_first_blocked);
        
    return 0;
}
