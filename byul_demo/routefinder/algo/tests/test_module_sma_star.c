#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/sma_star.h"
#include <glib.h>
#include <stdio.h>

static void test_sma_star_basic(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    sma_star_config cfg = sma_star_config_new_full(100); // 충분한 메모리 허용

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_SMA_STAR,
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
    sma_star_config_free(cfg);
}

static void test_sma_star_blocked_trimmed(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    sma_star_config cfg = sma_star_config_new_full(10); // 의도적으로 낮게 설정

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_SMA_STAR,
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
    g_assert_true(route_get_success(p)); // 제한 내에서 우회 성공 가능

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    sma_star_config_free(cfg);
}

static void test_sma_star_blocked_trimmed_5(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    sma_star_config cfg = sma_star_config_new_full(5); // 의도적으로 낮게 설정

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_SMA_STAR,
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
    g_assert_true(route_get_success(p)); // 제한 내에서 우회 성공 가능

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    sma_star_config_free(cfg);
}

static void test_sma_star_blocked_trimmed_2(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    sma_star_config cfg = sma_star_config_new_full(2); // 의도적으로 낮게 설정

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_SMA_STAR,
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
    g_assert_true(route_get_success(p)); // 제한 내에서 우회 성공 가능

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    sma_star_config_free(cfg);
}

static void test_sma_star_blocked_trimmed_1(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    sma_star_config cfg = sma_star_config_new_full(1); // 의도적으로 낮게 설정

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_SMA_STAR,
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
    g_assert_true(route_get_success(p)); // 제한 내에서 우회 성공 가능

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    sma_star_config_free(cfg);
}

static void test_sma_star_blocked_trimmed_0(void) {
    coord start = coord_new_full(0, 0);
    coord goal   = coord_new_full(9, 9);

    sma_star_config cfg = sma_star_config_new_full(0); // 의도적으로 낮게 설정

    algo al = algo_new_full(
        10, 10,
        MAP_NEIGHBOR_8,
        PATH_ALGO_SMA_STAR,
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
    g_assert_false(route_get_success(p)); // 제한 내에서 우회 성공 가능

    map_print_ascii_with_visited_count(al->m, p, start, goal);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
    sma_star_config_free(cfg);
}

int run_sma_star_tests(void) {
    g_test_add_func("/sma_star/basic",   test_sma_star_basic);

    g_test_add_func("/sma_star/blocked_trimmed", 
        test_sma_star_blocked_trimmed);

    g_test_add_func("/sma_star/blocked_trimmed_5", 
        test_sma_star_blocked_trimmed_5);

    g_test_add_func("/sma_star/blocked_trimmed_2", 
        test_sma_star_blocked_trimmed_2);

    g_test_add_func("/sma_star/blocked_trimmed_1", 
        test_sma_star_blocked_trimmed_1);        

    g_test_add_func("/sma_star/blocked_trimmed_0", 
        test_sma_star_blocked_trimmed_0);        

    return 0;
}
