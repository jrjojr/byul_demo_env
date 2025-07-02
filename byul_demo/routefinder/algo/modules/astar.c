#include "internal/astar.h"
#include <glib.h>
#include <math.h>
#include "internal/algo.h"
#include "internal/algo_utils.h"

route astar_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    algo_reset(al);
    if (!al->frontier)
        al->frontier = NULL;

    route result = route_new();
    algo_set_cost_so_far(al, start, 0.0f);

    gfloat h_start = al->heuristic_fn
        ? al->heuristic_fn(start, goal, al->userdata)
        : default_heuristic(start, goal, al->userdata);

    algo_push_frontier(al, start, h_start);
    algo_add_visited(al, start);
    if (al->visited_logging) route_add_visited(result, start);

    gboolean found = FALSE;

    while (!algo_is_frontier_empty(al)) {
        coord current = algo_pop_frontier(al);

        if (coord_equal(current, goal)) {
            found = TRUE;
            coord_free(current);
            break;
        }

        gfloat current_cost = 0.0f;
        algo_get_cost_so_far(al, current, &current_cost);

        GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
        for (GList* l = neighbors; l; l = l->next) {
            coord next = l->data;

            gfloat move_cost = al->cost_fn
                ? al->cost_fn(al->m, current, next, al->userdata)
                : 1.0f;

            gfloat new_cost = current_cost + move_cost;

            gfloat known_cost;
            gboolean has_cost = algo_get_cost_so_far(al, next, &known_cost);

            if (!has_cost || new_cost < known_cost) {
                algo_set_cost_so_far(al, next, new_cost);

                gfloat h = al->heuristic_fn
                    ? al->heuristic_fn(next, goal, al->userdata)
                    : default_heuristic(next, goal, al->userdata);

                gfloat f = new_cost + h;
                algo_push_frontier(al, next, f);

                algo_insert_came_from(al, next, current);
                algo_add_visited(al, next);
                if (al->visited_logging) route_add_visited(result, next);
            }
        }

        free_coord_list(neighbors);
        coord_free(current);
    }

    if (found) {
        GList* reversed = NULL;
        // coord current = coord_copy(goal);
        coord current = goal;

        while (!coord_equal(current, start)) {
            reversed = prepend_coord_to_list(reversed, current);
            coord prev = algo_lookup_came_from(al, current);
            if (!prev) break;
            // current = coord_copy(prev);
            current = prev;
        }

        // reversed = algo_route_prepend_coord(reversed, start);
        reversed = prepend_coord_to_list(reversed, start);

        for (GList* l = reversed; l != NULL; l = l->next)
            route_add_coord(result, l->data);

        route_set_success(result, TRUE);
        // algo_coord_list_free(reversed);
        free_coord_list(reversed);
        // coord_free(current);
    } else {
        route_set_success(result, FALSE);
    }

    return result;
}
