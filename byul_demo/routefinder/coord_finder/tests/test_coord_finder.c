#include <glib.h>
#include <stdio.h>

#include "internal/coord_finder.h"

static gboolean test_map[10][10];

static gboolean is_reachable_cb(const coord_t* c, gpointer user_data) {
    if (c->x < 0 || c->x >= 10 || c->y < 0 || c->y >= 10)
        return FALSE;
    return test_map[c->y][c->x];
}

static void setup_map() {
    // 모든 셀을 막음
    for (int y = 0; y < 10; ++y)
        for (int x = 0; x < 10; ++x)
            test_map[y][x] = FALSE;

    // 일부 셀만 열어둠 (4,4) 주변만 열려있음
    test_map[4][4] = TRUE;
    test_map[4][5] = TRUE;
    test_map[5][4] = TRUE;
    test_map[3][4] = TRUE;
    test_map[4][3] = TRUE;
}

static void test_find_goal_bfs(void) {
    setup_map();
    coord_t* start = coord_new_full(2, 2);
    // coord_t* result = coord_new_full(-1, -1);
    coord_t* result = NULL;

    gboolean found = find_goal_bfs(start, is_reachable_cb, NULL, 10, &result);
    g_assert_true(found);
    g_assert_true(is_reachable_cb(result, NULL));
    printf("[BFS] found coord_t*: (%d, %d)\n", result->x, result->y);

        coord_free(start);
    coord_free(result);
}

static void test_find_goal_astar(void) {
    setup_map();
    coord_t* start = coord_new_full(2, 2);
    coord_t* result = NULL;

    gboolean found = find_goal_astar(start, is_reachable_cb, NULL, 10, &result);
    g_assert_true(found);
    g_assert_true(is_reachable_cb(result, NULL));
    printf("[A*]  found coord_t*: (%d, %d)\n", result->x, result->y);

    coord_free(start);
    coord_free(result);
}

int main(int argc, char *argv[]) {
    g_test_init(&argc, &argv, NULL);

    g_test_add_func("/coord_finder/bfs", test_find_goal_bfs);
    g_test_add_func("/coord_finder/astar", test_find_goal_astar);

    return g_test_run();
}
