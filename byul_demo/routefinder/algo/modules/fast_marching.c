#include "internal/fast_marching.h"
#include "internal/algo_utils.h"
#include "internal/map.h"
#include "internal/route.h"
#include <float.h>

route fast_marching_find(const algo al, const coord start, const coord goal) {
    if (!al || !start || !goal || !al->m) return NULL;

    algo_reset(al);
    route result = route_new();

    // 시작점 설정
    algo_push_frontier(al, start, 0.0f);
    algo_add_visited(al, start);
    if (al->visited_logging) route_add_visited(result, start);
    algo_set_cost_so_far(al, start, 0.0f);

    while (!algo_is_frontier_empty(al)) {
        coord current = algo_pop_frontier(al);
        if (!current) break;

        // 도착점 도달 시 경로 재구성
        if (coord_equal(current, goal)) {
            route_set_success(result, TRUE);
            algo_reconstruct_route(al, result, start, goal);
            coord_free(current);
            return result;
        }

        gfloat g = 0.0f;
        algo_get_cost_so_far(al, current, &g);

        GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
        for (GList* l = neighbors; l; l = l->next) {
            coord next = l->data;
            if (algo_contains_visited(al, next)) continue;

            gfloat cost = al->cost_fn
                ? al->cost_fn(al->m, current, next, al->userdata)
                : 1.0f;

            gfloat new_cost = g + cost;

            gfloat old_cost = FLT_MAX;
            gboolean has_old = algo_get_cost_so_far(al, next, &old_cost);

            if (!has_old || new_cost < old_cost) {
                algo_set_cost_so_far(al, next, new_cost);
                algo_insert_came_from(al, next, current);
                algo_push_frontier(al, next, new_cost);  // 거리 기반 정렬
                algo_add_visited(al, next);
                if (al->visited_logging) route_add_visited(result, next);
            }
        }

        free_coord_list(neighbors);
        coord_free(current);
    }

    route_set_success(result, FALSE);
    return result;
}
