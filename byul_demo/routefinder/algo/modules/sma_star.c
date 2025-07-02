#include "internal/sma_star.h"
#include "internal/algo_utils.h"
#include "internal/map.h"
#include "internal/route.h"
#include <float.h>

sma_star_config sma_star_config_new() {
    return sma_star_config_new_full(1000);
}

sma_star_config sma_star_config_new_full(gint memory_limit) {
    sma_star_config cfg = g_malloc0(sizeof(sma_star_config_t));
    if (!cfg) return NULL;
    cfg->memory_limit = memory_limit;
    return cfg;
}

void sma_star_config_free(sma_star_config cfg) {
    if (cfg) g_free(cfg);
}

route sma_star_find(const algo al, const coord start, const coord goal) {
    if (!al || !al->m || !start || !goal) return NULL;

    algo_reset(al);
    route result = route_new();

    sma_star_config cfg = (sma_star_config)al->algo_specific;
    gint memory_limit = cfg ? cfg->memory_limit : 1000;

    algo_push_frontier(al, start, 0.0f);  // 시작점 g=0
    algo_add_visited(al, start);
    algo_set_cost_so_far(al, start, 0.0f);
    if (al->visited_logging) route_add_visited(result, start);

    while (!algo_is_frontier_empty(al)) {
        coord current = algo_pop_frontier(al);
        if (!current) break;

        if (coord_equal(current, goal)) {
            coord_free(current);
            route_set_success(result, TRUE);
            algo_reconstruct_route(al, result, start, goal);
            return result;
        }

        gfloat g = 0.0f;
        algo_get_cost_so_far(al, current, &g);

        GList* neighbors = map_clone_neighbors(al->m, current->x, current->y);
        for (GList* l = neighbors; l != NULL; l = l->next) {
            coord next = l->data;
            if (algo_contains_visited(al, next)) continue;

            gfloat cost = al->cost_fn
                ? al->cost_fn(al->m, current, next, al->userdata)
                : 1.0f;
            gfloat new_cost = g + cost;

            gfloat h = al->heuristic_fn
                ? al->heuristic_fn(next, goal, al->userdata)
                : default_heuristic(next, goal, al->userdata);

            gfloat f = new_cost + h;

            algo_set_cost_so_far(al, next, new_cost);
            algo_insert_came_from(al, next, current);
            algo_push_frontier(al, next, f);
            algo_add_visited(al, next);
            if (al->visited_logging) route_add_visited(result, next);
        }

        free_coord_list(neighbors);
        coord_free(current);

        // ⭐ 메모리 제한 적용: frontier 크기 초과 시 제거
        if (algo_frontier_size(al) > memory_limit) {
            algo_trim_frontier(al); // 가장 f가 큰 노드 제거 (선택적으로 구현)
        }
    }

    route_set_success(result, FALSE);
    return result;
}
