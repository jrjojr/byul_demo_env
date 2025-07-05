
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

// extern "C" {
#include "routefinder.h"
#include "internal/algo_utils.h"
// }

TEST_CASE("Simple route_t* with algo_find") {
    algo al = algo_new_default(10, 10, ROUTE_ALGO_BFS, TRUE);
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal = coord_new_full(9, 9);

    route_t* p = algo_find(al, start, goal);

    CHECK(p != NULL);
    CHECK(route_get_success(p));
    CHECK(p->coords != NULL);

    coord_t* c_start = (coord_t*)g_list_first(p->coords)->data;
    coord_t* c_end = (coord_t*)g_list_last(p->coords)->data;

    CHECK(coord_equal(c_start, start));
    CHECK(coord_equal(c_end, goal));

    route_free(p);
    coord_free(start);
    coord_free(goal);
    algo_free(al);
}

TEST_CASE("find_astar") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_astar(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_bfs") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_bfs(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_dfs") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_dfs(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_dijkstra") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_dijkstra(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_fast_marching") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_fast_marching(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_fringe_search") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_fringe_search(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_greedy_best_first") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_greedy_best_first(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_ida_star") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_ida_star(start, goal);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_rta_star") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_rta_star(start, goal, 5);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_sma_star") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_sma_star(start, goal, 100);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

TEST_CASE("find_weighted_astar") {
    coord_t* start = coord_new_full(0, 0);
    coord_t* goal  = coord_new_full(9, 9);

    route_t* p = find_weighted_astar(start, goal, 2.0f);
    CHECK(p != NULL);
    CHECK(route_get_success(p));
    route_free(p);
    coord_free(start);
    coord_free(goal);
}

int main(int argc, char** argv) {
    doctest::Context context;
    context.applyCommandLine(argc, argv);
    return context.run();
}
