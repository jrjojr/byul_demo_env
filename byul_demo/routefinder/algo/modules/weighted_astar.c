#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/algo_utils.h"
#include "internal/weighted_astar.h"
#include <glib.h>
#include <math.h>

// 가중치 계수 1.0으로 생성
weighted_astar_config weighted_astar_config_new() {
    return weighted_astar_config_new_full(1.0f);
}

// 원하는 가중치 생성
weighted_astar_config weighted_astar_config_new_full(gfloat weight) {
    weighted_astar_config cfg = g_malloc0(sizeof(weighted_astar_config_t));
    if (!cfg) return NULL;
    cfg->weight = weight;
    return cfg;
}

void weighted_astar_config_free(weighted_astar_config cfg) {
    if (cfg) g_free(cfg);
}

// route weighted_astar_find(const algo al, 
//     const coord start, const coord goal) {

//     gfloat weight = 1.0f;
//     if (al && al->algo_specific) {
//         weighted_astar_config cfg = (weighted_astar_config)al->algo_specific;
//         weight = cfg->weight;
//     }

//     route p = route_new();
//     GList* frontier = NULL;
//     GHashTable* came_from = g_hash_table_new_full(
//         (GHashFunc) coord_hash, 
//         (GEqualFunc) coord_equal, 
//         (GDestroyNotify)coord_free, 
//         (GDestroyNotify)coord_free
//     );
//     GHashTable* cost_so_far = g_hash_table_new_full(
//         (GHashFunc) coord_hash, 
//         (GEqualFunc) coord_equal, 
//         (GDestroyNotify)coord_free, 
//         g_free);

//     // coord start = coord_copy(start);
//     // coord goal = coord_copy(goal);
//     coord start = start;
//     coord goal = goal;    

//     coord_pq start_item = g_new(coord_pq_t, 1);
//     start_item->c = coord_copy(start);
//     // start_item->c = start;
//     start_item->priority = 0.0f;
//     frontier = g_list_prepend(frontier, start_item);

//     g_hash_table_replace(came_from, coord_copy(start), NULL);
//     gfloat* start_cost = g_new(gfloat, 1);
//     *start_cost = 0.0f;
//     g_hash_table_replace(cost_so_far, coord_copy(start), start_cost);

//     while (frontier) {
//         coord_pq current_item = (coord_pq)frontier->data;
//         coord current = current_item->c;
//         // coord current = coord_copy(current_item->c);
//         frontier = g_list_delete_link(frontier, frontier);
//         g_free(current_item);

//         route_add_visited(p, current);

//         if (coord_equal(current, goal)) {
//             coord_free(current);
//             break;
//         }

//         GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
//         for (GList* l = neighbors; l != NULL; l = l->next) {
//             coord next = (coord)l->data;

// gfloat new_cost = *(gfloat*)g_hash_table_lookup(cost_so_far, current) +
//                     (al->cost_fn ? al->cost_fn(al->m, current, next, NULL) : 1.0f);
// gfloat* old_cost = (gfloat*)g_hash_table_lookup(cost_so_far, next);

//             if (!old_cost || new_cost < *old_cost) {
//                 g_hash_table_replace(came_from, 
//                     coord_copy(next), coord_copy(current));

//                 gfloat* stored = g_new(gfloat, 1);
//                 *stored = new_cost;
//                 g_hash_table_replace(cost_so_far, coord_copy(next), stored);

//                 gfloat h = al->heuristic_fn ? \
//                     al->heuristic_fn(next, goal, NULL) : \
//                     default_heuristic(next, goal, NULL);

//                 gfloat priority = new_cost + weight * h;

//                 coord_pq item = g_new(coord_pq_t, 1);
//                 item->c = coord_copy(next);
//                 // item->c = next;
//                 item->priority = priority;

//                 gboolean inserted = FALSE;
//                 for (GList* iter = frontier; iter; iter = iter->next) {
//                     coord_pq* other = iter->data;
//                     if (pq_compare(item, other) < 0) {
//                         frontier = g_list_insert_before(frontier, iter, item);
//                         inserted = TRUE;
//                         break;
//                     }
//                 }
//                 if (!inserted) frontier = g_list_append(frontier, item);
//             }
//         }
//         g_list_free_full(neighbors, (GDestroyNotify)coord_free);
//         coord_free(current);
//     }

//     GList* rev_route = reconstruct_route(came_from, goal);
//     for (GList* l = rev_route; l != NULL; l = l->next) {
//         route_add_coord(p, (coord)l->data);
//     }
//     // g_list_free(rev_route);
//     g_list_free_full(rev_route, (GDestroyNotify) coord_free);

//     p->success = (p->coords != NULL);

//     g_hash_table_destroy(came_from);
//     g_hash_table_destroy(cost_so_far);
//     g_list_free_full(frontier, 
//        (GDestroyNotify) coord_pq_free);
//     // coord_free(goal);
//     // coord_free(start);

//     return p;
// }

route weighted_astar_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    algo_reset(al);  // 모든 내부 상태 초기화

    if (!al->frontier)
        al->frontier = NULL;

    gfloat weight = 1.0f;
    if (al->algo_specific) {
        weighted_astar_config cfg = (weighted_astar_config)al->algo_specific;
        weight = cfg->weight;
    }

    route result = route_new();
    algo_set_cost_so_far(al, start, 0.0f);

    gfloat h_start = al->heuristic_fn
        ? al->heuristic_fn(start, goal, al->userdata)
        : default_heuristic(start, goal, al->userdata);

    gfloat f_start = h_start * weight;

    algo_push_frontier(al, start, f_start);
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
                algo_insert_came_from(al, next, current);

                gfloat h = al->heuristic_fn
                    ? al->heuristic_fn(next, goal, al->userdata)
                    : default_heuristic(next, goal, al->userdata);

                gfloat f = new_cost + weight * h;
                algo_push_frontier(al, next, f);
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
