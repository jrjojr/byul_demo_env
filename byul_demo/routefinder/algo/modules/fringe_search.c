// #include "internal/fringe_search.h"
// #include "internal/algo_utils.h"
// #include "internal/map.h"
// #include "internal/route.h"
// #include <float.h>

// route fringe_search_find(const algo al, const coord start, const coord goal) {
//     if (!al || !al->m || !start || !goal) return NULL;
//     if (!al->heuristic_fn) return NULL;

//     algo_reset(al);
//     route result = route_new();

//     gfloat threshold = al->heuristic_fn(start, goal, al->userdata);

//     gfloat g_start = 0.0f;
//     gfloat f_start = g_start + threshold;

//     algo_push_frontier(al, start, f_start);
//     algo_add_visited(al, start);
//     if (al->visited_logging) route_add_visited(result, start);
//     algo_set_cost_so_far(al, start, g_start);

//     gboolean found = FALSE;

//     while (!algo_is_frontier_empty(al)) {
//         GList* next = NULL;
//         gfloat next_threshold = FLT_MAX;

//         while (!algo_is_frontier_empty(al)) {
//             coord current = algo_pop_frontier(al);
//             if (!current) break;

//             gfloat g = 0.0f;
//             algo_get_cost_so_far(al, current, &g);

//             gfloat h = al->heuristic_fn(current, goal, al->userdata);
//             gfloat f = g + h;

//             if (f > threshold) {
//                 if (f < next_threshold) next_threshold = f;

//                 coord_pq node = coord_pq_new_full(current, f);
//                 next = append_coord_pq_to_list(next, node);
//                 coord_free(current);

// g_print("f=%.2f, threshold=%.2f, next_threshold=%.2f, next_len=%d\n",
//         f, threshold, next_threshold, g_list_length(next));

//                 continue;
//             }

//             if (coord_equal(current, goal)) {

// g_print("==> Goal reached\n");

//                 found = TRUE;
//                 coord_free(current);
//                 break;
//             }

//             GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
//             for (GList* l = neighbors; l; l = l->next) {
//                 coord next_c = l->data;

//                 gfloat cost = al->cost_fn
//                     ? al->cost_fn(al->m, current, next_c, al->userdata)
//                     : 1.0f;

//                 gfloat new_g = g + cost;
//                 gfloat old_g = FLT_MAX;
//                 gboolean has_old = algo_get_cost_so_far(al, next_c, &old_g);

//                 if (!has_old || new_g < old_g) {
//                     algo_set_cost_so_far(al, next_c, new_g);
//                     algo_insert_came_from(al, next_c, current);

//                     gfloat new_f = new_g + al->heuristic_fn(next_c, goal, al->userdata);
//                     algo_push_frontier(al, next_c, new_f);
//                     algo_add_visited(al, next_c);
//                     if (al->visited_logging) route_add_visited(result, next_c);
//                 }
//             }

//             free_coord_list(neighbors);
//             coord_free(current);
//         }

//         if (found) break;

//         algo_free_frontier(al);
//         for (GList* l = next; l; l = l->next) {
//             coord_pq item = l->data;
//             algo_push_frontier(al, item->c, item->priority);
//             coord_free(item->c);
//             g_free(item);
//         }
//         g_list_free(next);
//         threshold = next_threshold;
//     }

//     if (found) {
//         route_set_success(result, TRUE);
//         algo_reconstruct_route(al, result, start, goal);
//     } else {
//         route_set_success(result, FALSE);
//     }

//     return result;
// }

// #include "internal/fringe_search.h"
// #include "internal/algo_utils.h"
// #include "internal/map.h"
// #include "internal/route.h"
// #include <float.h>

// #define FRINGE_DELTA_EPSILON 0.01f
// #define FRINGE_DELTA_EPSILON 0.5f
// #define FRINGE_DELTA_EPSILON 1.0f
// #define FRINGE_DELTA_EPSILON 20.0f
// #define FRINGE_DELTA_EPSILON 3.0f

// route fringe_search_find(const algo al, const coord start, const coord goal) {
//     if (!al || !al->m || !start || !goal) return NULL;
//     if (!al->heuristic_fn) return NULL;

//     algo_reset(al);
//     route result = route_new();

//     gfloat threshold = al->heuristic_fn(start, goal, al->userdata);

//     gfloat g_start = 0.0f;
//     gfloat f_start = g_start + threshold;

//     algo_push_frontier(al, start, f_start);
//     algo_add_visited(al, start);
//     if (al->visited_logging) route_add_visited(result, start);
//     algo_set_cost_so_far(al, start, g_start);

//     gboolean found = FALSE;

//     while (!algo_is_frontier_empty(al)) {
//         GList* next = NULL;
//         gfloat next_threshold = FLT_MAX;

//         while (!algo_is_frontier_empty(al)) {
//             coord current = algo_pop_frontier(al);
//             if (!current) break;

//             gfloat g = 0.0f;
//             algo_get_cost_so_far(al, current, &g);

//             gfloat h = al->heuristic_fn(current, goal, al->userdata);
//             gfloat f = g + h;

//     // g_print("f=%.2f, threshold=%.2f, next_threshold=%.2f, next_len=%d\n",
//     //         f, threshold, next_threshold, g_list_length(next));            

//             if (f > threshold + FRINGE_DELTA_EPSILON) {
//                 if (f < next_threshold) next_threshold = f;

//                 coord_pq node = coord_pq_new_full(current, f);
//                 next = append_coord_pq_to_list(next, node);
//                 coord_free(current);
//                 coord_pq_free(node);

//     // g_print("f=%.2f, threshold=%.2f, next_threshold=%.2f, next_len=%d\n",
//     //         f, threshold, next_threshold, g_list_length(next));

//                 continue;
//             }

//             if (coord_equal(current, goal)) {
//                 g_print("==> Goal reached\n");
//                 found = TRUE;
//                 coord_free(current);
//                 break;
//             }

//             GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
//             for (GList* l = neighbors; l; l = l->next) {
//                 coord next_c = l->data;

//                 gfloat cost = al->cost_fn
//                     ? al->cost_fn(al->m, current, next_c, al->userdata)
//                     : 1.0f;

//                 gfloat new_g = g + cost;
//                 gfloat old_g = FLT_MAX;
//                 gboolean has_old = algo_get_cost_so_far(al, next_c, &old_g);

//                 if (!has_old || new_g < old_g) {
//                     algo_set_cost_so_far(al, next_c, new_g);
//                     algo_insert_came_from(al, next_c, current);

//                     gfloat new_f = new_g + al->heuristic_fn(next_c, goal, al->userdata);
//                     algo_push_frontier(al, next_c, new_f);
                    
//                     algo_add_visited(al, next_c);
//                     if (al->visited_logging) route_add_visited(result, next_c);

// // if (f <= threshold + FRINGE_DELTA_EPSILON) {
// //     algo_add_visited(al, next_c);
// //     if (al->visited_logging) route_add_visited(result, next_c);
// // }

//                 }
//             }

//             free_coord_list(neighbors);
//             coord_free(current);
//         }

//         if (found) break;

//         algo_free_frontier(al);
//         for (GList* l = next; l; l = l->next) {
//             coord_pq item = l->data;
//             algo_push_frontier(al, item->c, item->priority);
//             coord_free(item->c);
//             g_free(item);
//         }
//         g_list_free(next);
//         threshold = next_threshold;
//     }

//     if (found) {
//         route_set_success(result, TRUE);
//         algo_reconstruct_route(al, result, start, goal);
//     } else {
//         route_set_success(result, FALSE);
//     }

//     return result;
// }

#include "internal/fringe_search.h"
#include "internal/algo_utils.h"
#include "internal/map.h"
#include "internal/route.h"
#include <float.h>

fringe_search_config fringe_search_config_new(void) {
    return fringe_search_config_new_full(0.5f);  // 기본값
}

fringe_search_config fringe_search_config_new_full(gfloat delta) {
    fringe_search_config cfg = g_malloc0(sizeof(fringe_search_config_t));
    if (cfg) cfg->delta_epsilon = delta;
    return cfg;
}

void fringe_search_config_free(fringe_search_config cfg) {
    if (cfg) g_free(cfg);
}


route fringe_search_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;
    if (!al->heuristic_fn) return NULL;

    fringe_search_config cfg = (fringe_search_config)al->algo_specific;
    gfloat delta = cfg ? cfg->delta_epsilon : 0.5f;

    algo_reset(al);
    route result = route_new();

    gfloat threshold = al->heuristic_fn(start, goal, al->userdata);

    gfloat g_start = 0.0f;
    gfloat f_start = g_start + threshold;

    algo_push_frontier(al, start, f_start);
    algo_set_cost_so_far(al, start, g_start);
    algo_add_visited(al, start);
    if (al->visited_logging) route_add_visited(result, start);

    gboolean found = FALSE;

    while (!algo_is_frontier_empty(al)) {
        GList* next = NULL;
        gfloat next_threshold = FLT_MAX;
        gboolean expanded = FALSE;

        while (!algo_is_frontier_empty(al)) {
            coord current = algo_pop_frontier(al);
            if (!current) break;

            gfloat g = 0.0f;
            algo_get_cost_so_far(al, current, &g);

            gfloat h = al->heuristic_fn(current, goal, al->userdata);
            gfloat f = g + h;

            if (f > threshold + delta) {
                if (f < next_threshold) next_threshold = f;
                coord_pq node = coord_pq_new_full(current, f);
                next = append_coord_pq_to_list(next, node);
                coord_free(current);
                coord_pq_free(node);
                continue;
            }

            if (coord_equal(current, goal)) {
                // g_print("==> Goal reached\n");
                found = TRUE;
                coord_free(current);
                break;
            }

            GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
            for (GList* l = neighbors; l; l = l->next) {
                coord next_c = l->data;

                gfloat cost = al->cost_fn
                    ? al->cost_fn(al->m, current, next_c, al->userdata)
                    : 1.0f;

                gfloat new_g = g + cost;
                gfloat old_g = FLT_MAX;
                gboolean has_old = algo_get_cost_so_far(al, next_c, &old_g);

                if (!has_old || new_g < old_g) {
                    algo_set_cost_so_far(al, next_c, new_g);
                    algo_insert_came_from(al, next_c, current);

                    gfloat new_f = new_g + al->heuristic_fn(next_c, goal, al->userdata);
                    algo_push_frontier(al, next_c, new_f);

                    if (new_f <= threshold + delta) {
                        algo_add_visited(al, next_c);
                        if (al->visited_logging) route_add_visited(result, next_c);
                        expanded = TRUE;
                    }
                }
            }

            coord_free(current);
            free_coord_list(neighbors);
        }

        if (found) break;

        if (!next || g_list_length(next) == 0 || !expanded) {
            route_set_success(result, FALSE);
            break;
        }

        algo_free_frontier(al);
        for (GList* l = next; l; l = l->next) {
            coord_pq item = l->data;
            algo_push_frontier(al, item->c, item->priority);
            coord_free(item->c);
            g_free(item);
        }
        g_list_free(next);

        if (next_threshold <= threshold + delta) {
            threshold += 1.0f;
        } else {
            threshold = next_threshold;
        }
    }

    if (found) {
        route_set_success(result, TRUE);
        algo_reconstruct_route(al, result, start, goal);
    } else {
        route_set_success(result, FALSE);
    }

    return result;
}
