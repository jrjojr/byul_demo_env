#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo_utils.h"
#include "internal/greedy_best_first.h"
#include <glib.h>
#include <math.h>

route greedy_best_first_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    algo_reset(al);  // 내부 상태 초기화

    if (!al->frontier)
        al->frontier = NULL;

    route result = route_new();

    gfloat h_start = al->heuristic_fn
        ? al->heuristic_fn(start, goal, al->userdata)
        : default_heuristic(start, goal, al->userdata);

    gfloat f_start = h_start;  // g는 무시

    algo_push_frontier(al, start, f_start);
    algo_add_visited(al, start);
    if (al->visited_logging)
        route_add_visited(result, start);

    gboolean found = FALSE;

    while (!algo_is_frontier_empty(al)) {
        coord current = algo_pop_frontier(al);

        if (coord_equal(current, goal)) {
            found = TRUE;
            coord_free(current);
            break;
        }

        GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
        for (GList* l = neighbors; l; l = l->next) {
            coord next = l->data;

            if (algo_contains_visited(al, next)) continue;

            gfloat h = al->heuristic_fn
                ? al->heuristic_fn(next, goal, al->userdata)
                : default_heuristic(next, goal, al->userdata);

            gfloat f = h;  // g값 없이 h만 사용

            algo_push_frontier(al, next, f);
            algo_insert_came_from(al, next, current);
            algo_add_visited(al, next);
            if (al->visited_logging)
                route_add_visited(result, next);
        }

        free_coord_list(neighbors);
        coord_free(current);
    }

    if (found) {
        GList* reversed = NULL;
        coord current = goal;

        while (!coord_equal(current, start)) {
            reversed = prepend_coord_to_list(reversed, current);
            coord prev = algo_lookup_came_from(al, current);
            if (!prev) break;
            current = prev;
        }

        reversed = prepend_coord_to_list(reversed, start);

        for (GList* l = reversed; l; l = l->next)
            route_add_coord(result, l->data);

        route_set_success(result, TRUE);
        free_coord_list(reversed);
    } else {
        route_set_success(result, FALSE);
    }

    return result;
}
