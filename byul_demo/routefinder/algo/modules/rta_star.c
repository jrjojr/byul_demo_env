#include "internal/rta_star.h"
#include "internal/algo_utils.h"
#include "internal/map.h"
#include "internal/route.h"
#include <math.h>
#include <float.h>

rta_star_config rta_star_config_new() {
    return rta_star_config_new_full(3); // 기본 깊이 제한
}

rta_star_config rta_star_config_new_full(gint depth_limit) {
    rta_star_config cfg = g_malloc0(sizeof(rta_star_config_t));
    if (!cfg) return NULL;
    cfg->depth_limit = depth_limit;
    return cfg;
}

void rta_star_config_free(rta_star_config cfg) {
    if (cfg) g_free(cfg);
}

// 반복문 기반 lookahead 평가 함수
static gfloat rta_iterative_eval(const algo al, 
    const coord start, const coord goal, gint max_depth) {
        
    coord current = coord_copy(start);
    gfloat g = 0.0f;

    for (int d = 0; d < max_depth; d++) {
        if (coord_equal(current, goal)) break;

        GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
        coord best = NULL;
        gfloat best_f = FLT_MAX;

        for (GList* l = neighbors; l; l = l->next) {
            coord next = l->data;

            gfloat cost = al->cost_fn
                ? al->cost_fn(al->m, current, next, al->userdata)
                : 1.0f;

            gfloat h = al->heuristic_fn
                ? al->heuristic_fn(next, goal, al->userdata)
                : default_heuristic(next, goal, al->userdata);

            gfloat f = g + cost + h;
            if (f < best_f) {
                best_f = f;
                if (best) coord_free(best);
                best = coord_copy(next);
            }
        }

        free_coord_list(neighbors);
        if (!best) break;

        gfloat move_cost = al->cost_fn
            ? al->cost_fn(al->m, current, best, al->userdata)
            : 1.0f;
        g += move_cost;

        coord_free(current);
        current = best;
        best = NULL;
    }

    gfloat h_final = al->heuristic_fn
        ? al->heuristic_fn(current, goal, al->userdata)
        : default_heuristic(current, goal, al->userdata);

    gfloat total_f = g + h_final;
    coord_free(current);
    return total_f;
}

route rta_star_find(const algo al, const coord start, const coord goal) {
    if (!al || !start || !goal) return NULL;

    algo_reset(al);
    route result = route_new();

    if (!al->heuristic_fn) {
        route_set_success(result, FALSE);
        return result;
    }

    rta_star_config cfg = (rta_star_config)al->algo_specific;
    gint max_depth = (cfg ? cfg->depth_limit : 3);

    coord current = coord_copy(start);
    route_add_coord(result, current);
    algo_add_visited(al, current);
    if (al->visited_logging)
        route_add_visited(result, current);

    while (!coord_equal(current, goal)) {
        GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
        if (!neighbors) break;

        coord best = NULL;
        gfloat best_f = FLT_MAX;

        for (GList* l = neighbors; l != NULL; l = l->next) {
            coord next = l->data;

            if (algo_contains_visited(al, next)) continue;

            gfloat eval = rta_iterative_eval(al, next, goal, max_depth - 1);
            if (eval < best_f) {
                best_f = eval;
                if (best) coord_free(best);
                best = coord_copy(next);
            }
        }

        free_coord_list(neighbors);
        if (!best) break;

        coord_free(current);
        current = best;
        best = NULL;

        route_add_coord(result, current);
        algo_add_visited(al, current);
        if (al->visited_logging)
            route_add_visited(result, current);
    }

    if (coord_equal(current, goal)) {
        route_set_success(result, TRUE);
    } else {
        route_set_success(result, FALSE);
    }

    coord_free(current);
    return result;
}
