#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo_utils.h"
#include "internal/ida_star.h"
#include <glib.h>
#include <math.h>

route ida_star_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    route result = route_new();

    gfloat threshold = al->heuristic_fn
        ? al->heuristic_fn(start, goal, al->userdata)
        : default_heuristic(start, goal, al->userdata);

    while (TRUE) {
        gfloat next_threshold = G_MAXFLOAT;

        // 내부 상태 초기화 (frontier, visited, came_from, cost_so_far)
        algo_reset(al);

        algo_push_frontier(al, start, 0.0f);  // g = 0.0
        algo_add_visited(al, start);
        algo_set_cost_so_far(al, start, 0.0f);

        gboolean found = FALSE;

        while (!algo_is_frontier_empty(al)) {
            coord current = algo_pop_frontier(al);
            if (!current) break;

            gfloat g_cost = 0.0f;
            algo_get_cost_so_far(al, current, &g_cost);

            gfloat h = al->heuristic_fn
                ? al->heuristic_fn(current, goal, al->userdata)
                : default_heuristic(current, goal, al->userdata);

            gfloat f = g_cost + h;
            if (f > threshold) {
                if (f < next_threshold) next_threshold = f;
                coord_free(current);
                continue;
            }

            if (al->visited_logging)
                route_add_visited(result, current);

            if (coord_equal(current, goal)) {
                found = TRUE;
                coord_free(current);
                break;
            }

            GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
            for (GList* l = neighbors; l; l = l->next) {
                coord next = l->data;

                if (algo_contains_visited(al, next)) continue;

                gfloat move_cost = al->cost_fn
                    ? al->cost_fn(al->m, current, next, al->userdata)
                    : 1.0f;

                gfloat new_cost = g_cost + move_cost;

                algo_set_cost_so_far(al, next, new_cost);
                algo_insert_came_from(al, next, current);
                algo_push_frontier(al, next, new_cost);
                algo_add_visited(al, next);
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
            return result;
        }

        if (next_threshold == G_MAXFLOAT) break;
        threshold = next_threshold;
    }

    route_set_success(result, FALSE);
    return result;
}
