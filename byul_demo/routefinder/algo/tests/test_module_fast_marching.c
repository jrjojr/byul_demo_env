#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/fast_marching.h"
#include <glib.h>
#include <stdio.h>

static void test_fast_marching_basic(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FAST_MARCHING,
        default_cost,
        NULL,           // 휴리스틱 사용 안 함
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

static void test_fast_marching_blocked(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_FAST_MARCHING,
        default_cost,
        NULL,
        NULL,
        NULL,
        TRUE
    );

    // 중앙 수직 장애물
    for (int y = 1; y < 10; y++)
        map_block_coord(al->m, 5, y);

    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p)); // 우회 가능한지 확인

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

int run_fast_marching_tests(void) {
    g_test_add_func("/fast_marching/basic", test_fast_marching_basic);
    g_test_add_func("/fast_marching/blocked", test_fast_marching_blocked);
    return 0;
}
