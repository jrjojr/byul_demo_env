#include <glib.h>
#include "routefinder.h"
#include "internal/algo_utils.h"

// Helper macro
#define COORD_EQUAL(a, b) coord_equal((a), (b))

static void test_algo_find_bfs(void) {
    algo al = algo_new_default(10, 10, PATH_ALGO_BFS, true);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = algo_find(al, start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    g_assert_nonnull(p->coords);


    coord_t* c_start = (coord_t*) g_list_first(p->coords)->data;
    coord_t* c_end = (coord_t*) g_list_last(p->coords)->data;

    g_assert_true(COORD_EQUAL(c_start, start));
    g_assert_true(COORD_EQUAL(c_end, goal));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

static void test_find_astar(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_astar(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_bfs(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_bfs(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_dfs(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_dfs(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_dijkstra(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_dijkstra(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_fast_marching(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_fast_marching(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_fringe_search(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_fringe_search(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_greedy_best_first(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_greedy_best_first(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_ida_star(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_ida_star(start, goal);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_rta_star(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_rta_star(start, goal, 5);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_sma_star(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_sma_star(start, goal, 100);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

static void test_find_weighted_astar(void) {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_weighted_astar(start, goal, 2.0f);

    g_assert_nonnull(p);
    g_assert_true(route_get_success(p));
    route_print(p);

    route_free(p);
    coord_free(start);
    coord_free(goal);
}

int main(int argc, char **argv) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/route/algo_find",               test_algo_find_bfs);
    g_test_add_func("/route/find_astar",              test_find_astar);
    g_test_add_func("/route/find_bfs",                test_find_bfs);
    g_test_add_func("/route/find_dfs",                test_find_dfs);
    g_test_add_func("/route/find_dijkstra",           test_find_dijkstra);
    g_test_add_func("/route/find_fast_marching",      test_find_fast_marching);
    g_test_add_func("/route/find_fringe_search",      test_find_fringe_search);
    g_test_add_func("/route/find_greedy_best_first",  test_find_greedy_best_first);
    g_test_add_func("/route/find_ida_star",           test_find_ida_star);
    g_test_add_func("/route/find_rta_star",           test_find_rta_star);
    g_test_add_func("/route/find_sma_star",           test_find_sma_star);
    g_test_add_func("/route/find_weighted_astar",     test_find_weighted_astar);

    return g_test_run();
}
