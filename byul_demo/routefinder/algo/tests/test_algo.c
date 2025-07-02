#include <glib.h>
#include "core.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo.h"
#include "internal/algo_utils.h"
#include "internal/bfs.h"
#include <locale.h>

static void test_simple_route(void) {
    algo al = algo_new_default(10, 10, PATH_ALGO_BFS, TRUE);
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);

    // route p = bfs_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    g_assert(p->coords != NULL);
    g_assert(g_list_length(p->coords) > 0);

    coord c_start = g_list_first(p->coords)->data;
    coord c_end = g_list_last(p->coords)->data;

    g_print("[TEST] start: (%d, %d)\n", coord_get_x(start), coord_get_y(start));
    g_print("[TEST] actual: (%d, %d)\n", coord_get_x(c_start), coord_get_y(c_start));

    // ✅ 전체 경로 출력
    g_print("[TEST] 경로: ");
    for (GList* l = p->coords; l != NULL; l = l->next) {
        coord c = l->data;
        g_print("(%d, %d)", coord_get_x(c), coord_get_y(c));
        if (l->next) g_print(" → ");
    }
    g_print("\n");

    g_assert(coord_equal(c_start, start));
    g_assert(coord_equal(c_end, goal));

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

static void test_blocked_route(void) {
    algo al = algo_new_default(10, 10, PATH_ALGO_BFS, TRUE);
    coord start = coord_new_full(0, 0);
    coord goal = coord_new_full(9, 9);
g_assert_false(coord_equal(start, goal));  // 방어 assert
    // ✅ 장애물 삽입
    for (int y = 1; y < 10; y++) {
        // map_block(m, 1, y);  // (1,0)~(1,7) 막힘
        // map_block_coord(m, 5, y);
        map_block_coord(al->m, 5, y);
    }

    // route p = bfs_find_simple(m, start, goal);
    // route p = bfs_find(al, start, goal);
    route p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));

    g_assert(p->coords != NULL);
    g_assert(g_list_length(p->coords) > 0);

    // coord c_start = flud_get_coord(coords->data);
    // coord c_end = flud_get_coord(g_list_last(coords)->data);

// ✅ 좌표 직접 접근 (flud 제거)
    coord c_start = g_list_first(p->coords)->data;
    coord c_end = g_list_last(p->coords)->data;

    g_print("[TEST] start: (%d, %d)\n", coord_get_x(start), coord_get_y(start));
    g_print("[TEST] actual: (%d, %d)\n", coord_get_x(c_start), coord_get_y(c_start));

    // ✅ 경로 전체 출력
    g_print("[TEST] 경로: ");
    for (GList* l = p->coords; l != NULL; l = l->next) {
        // coord c = flud_get_coord(l->data);
        coord c = (coord)(l->data);

        g_print("(%d, %d)", coord_get_x(c), coord_get_y(c));
        if (l->next) g_print(" → ");
    }
    g_print("\n");

    g_assert(coord_equal(c_start, start));
    g_assert(coord_equal(c_end, goal));

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

    #include "test_module_map_print.c"
    #include "test_module_dfs.c"
    #include "test_module_dijkstra.c"
    #include "test_module_astar.c"
    #include "test_module_weighted_astar.c"
    #include "test_module_greedy_best_first.c"
    #include "test_module_ida_star.c"
    #include "test_module_rta_star.c"
    #include "test_module_sma_star.c"
    #include "test_module_fast_marching.c"
    #include "test_module_fringe_search.c"
    // #include "test_module_dstar_lite.c"

int main(int argc, char **argv) {
    setlocale(LC_ALL, "ko_KR.UTF-8");  // 💥 핵심
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/algo/simple_route", test_simple_route);
    g_test_add_func("/algo/blocked_route", test_blocked_route);

    run_map_print_tests();
    run_dfs_tests();
    run_dijkstra_tests();
    run_astar_tests();
    run_weighted_astar_tests();
    run_greedy_best_first_tests();
    run_ida_star_tests();
    run_rta_star_tests();
    run_sma_star_tests();
    run_fast_marching_tests();
    run_fringe_search_tests();
    // run_dstar_lite_tests();

    return g_test_run();
}
