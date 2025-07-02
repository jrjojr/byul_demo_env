#include "internal/dfs.h"
#include <glib.h>
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"

// route dfs_find_simple(const map m, const coord start, const coord goal) {
//     if (!m || !start || !goal) return NULL;

//     gboolean found = FALSE;
//     route result = route_new();

//     GQueue* stack = g_queue_new();  // DFS는 LIFO

//     GHashTable* came_from = g_hash_table_new_full(
//         (GHashFunc)coord_hash,
//         (GEqualFunc)coord_equal,
//         (GDestroyNotify)coord_free,
//         (GDestroyNotify)coord_free
//     );

//     g_queue_push_head(stack, coord_copy(start));

//     // g_hash_table_add(visited, coord_copy(start));
//     route_add_visited(result, start);

//     while (!g_queue_is_empty(stack)) {
//         coord current = g_queue_pop_head(stack);

//         if (coord_equal(current, goal)) {
//             found = TRUE;
//             coord_free(current);
//             break;
//         }

//         GList* neighbors = map_clone_neighbors(m, current->x, current->y);
//         for (GList* l = neighbors; l != NULL; l = l->next) {
//             coord neighbor = l->data;
//             if (!g_hash_table_contains(result->visited, neighbor)) {
//                 g_queue_push_head(stack, coord_copy(neighbor));
                
//                 // g_hash_table_add(result->visited, coord_copy(neighbor));
//                 route_add_visited(result, neighbor);

//                 g_hash_table_replace(came_from, coord_copy(neighbor), 
//                     coord_copy(current));
//             }
//         }

//         g_list_free_full(neighbors, (GDestroyNotify)coord_free);
//         coord_free(current);
//     }

//     if (found) {
//         GList* reversed = NULL;
//         coord current = coord_copy(goal);

//         while (!coord_equal(current, start)) {
//             reversed = g_list_prepend(reversed, current);
//             coord prev = g_hash_table_lookup(came_from, current);
//             current = coord_copy(prev);
//         }
//         reversed = g_list_prepend(reversed, coord_copy(start));

//         for (GList* l = reversed; l != NULL; l = l->next) {
//             route_add_coord(result, l->data);
//         }
//         route_set_success(result, TRUE);

//         g_list_free_full(reversed, (GDestroyNotify)coord_free);
//         coord_free(current);
//     }

//     while (!g_queue_is_empty(stack)) {
//         coord leftover = g_queue_pop_head(stack);
//         coord_free(leftover);
//     }

//     g_queue_free(stack);

//     g_hash_table_destroy(came_from);

//     return result;
// }

route dfs_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    map m = al->m;

    gboolean found = FALSE;
    route result = route_new();

    algo_reset(al);  // 내부 해시, 큐 초기화

    // DFS는 스택 → LIFO 구조
    if (!al->frontier)
        al->frontier = g_queue_new();

    algo_prepend_frontier(al, start);
    algo_add_visited(al, start);
    if (al->visited_logging) route_add_visited(result, start);

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
                algo_prepend_frontier(al, neighbor);
                algo_insert_came_from(al, neighbor, current);
                algo_add_visited(al, neighbor);
                if (al->visited_logging) route_add_visited(result, neighbor);
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
