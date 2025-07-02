#include "internal/dijkstra.h"
#include <glib.h>
#include "internal/algo.h"
#include "internal/algo_utils.h"

route dijkstra_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    algo_reset(al);

    if (!al->frontier)
        al->frontier = NULL;  // 우선순위 큐는 GList*

    map m = al->m;
    route result = route_new();
    algo_set_cost_so_far(al, start, 0.0f);
    algo_push_frontier(al, start, 0.0f);  // 초기 우선순위 = 비용 0
    algo_add_visited(al, start);
    if (al->visited_logging) route_add_visited(result, start);

    gboolean found = FALSE;

    while (!algo_is_frontier_empty(al)) {
        coord current = algo_pop_frontier(al);  // 최소 비용 pop

        if (coord_equal(current, goal)) {
            found = TRUE;
            coord_free(current);
            break;
        }

        gfloat current_cost = 0.0f;
        algo_get_cost_so_far(al, current, &current_cost);

        GList* neighbors = map_clone_neighbors(m, current->x, current->y);
        for (GList* l = neighbors; l != NULL; l = l->next) {
            coord next = l->data;
            gfloat move_cost = al->cost_fn
                ? al->cost_fn(m, current, next, al->userdata)
                : 1.0f;

            gfloat new_cost = current_cost + move_cost;

            gfloat known_cost;
            gboolean has_cost = algo_get_cost_so_far(al, next, &known_cost);

            if (!has_cost || new_cost < known_cost) {
                algo_set_cost_so_far(al, next, new_cost);
                algo_push_frontier(al, next, new_cost);  // 우선순위 = 누적 비용
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

        reversed = prepend_coord_to_list(reversed, start);

        for (GList* l = reversed; l != NULL; l = l->next)
            route_add_coord(result, l->data);

        route_set_success(result, TRUE);
        free_coord_list(reversed);
        // coord_free(current);
    } else {
        route_set_success(result, FALSE);
    }

    return result;
}
