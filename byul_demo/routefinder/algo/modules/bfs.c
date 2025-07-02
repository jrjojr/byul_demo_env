#include "internal/bfs.h"
#include <glib.h>
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo.h"
#include "internal/algo_utils.h"

route bfs_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    // map m = map_copy(al->m);
    map m = al->m;
    route result = route_new_full(0.0f);

    algo_reset(al);

    // 탐색 큐 초기화
    if (!al->frontier)
        al->frontier = g_queue_new();

    algo_append_frontier(al, start);
    algo_add_visited(al, start);
    
    if (al->visited_logging) route_add_visited(result, start);

    gboolean found = FALSE;

    while (!algo_is_frontier_empty(al)) {
        coord current = algo_pop_frontier_head(al);

        if (coord_equal(current, goal)) {
            found = TRUE;
            coord_free(current);
            break;
        }
        GList* neighbors = map_clone_neighbors(m, current->x, current->y);
        for (GList* l = neighbors; l != NULL; l = l->next) {
            coord neighbor = l->data;

            if (!algo_contains_came_from(al, neighbor)) {
                algo_append_frontier(al, neighbor);
                algo_insert_came_from(al, neighbor, current);
                algo_add_visited(al, neighbor);
                if (al->visited_logging) route_add_visited(result, neighbor);
            }
            // coord_free(neighbor);
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
            // current = coord_copy(algo_lookup_came_from(al, current));
            current = algo_lookup_came_from(al, current);
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
