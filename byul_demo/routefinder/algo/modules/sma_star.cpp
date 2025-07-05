#include "internal/sma_star.h"
#include "internal/map.h"
#include "coord.hpp"
#include "internal/coord_list.h"
#include "internal/coord_hash.h"
#include "internal/cost_coord_pq.h"
#include "internal/route.h"
#include "internal/algo_utils.h"
#include <float.h>
#include <cmath>

route_t* find_sma_star(const map_t* m,
    const coord_t* start, const coord_t* goal,
    cost_func cost_fn, heuristic_func heuristic_fn,
    int memory_limit, int max_retry, bool visited_logging) {

    if (!m || !start || !goal || memory_limit <= 0 || max_retry <= 0)
        return nullptr;

    if (!cost_fn) cost_fn = default_cost;
    if (!heuristic_fn) heuristic_fn = default_heuristic;

    route_t* result = route_new();
    coord_hash_t* cost_so_far = coord_hash_new();
    coord_hash_t* came_from = coord_hash_new();
    cost_coord_pq_t* frontier = cost_coord_pq_new();

    float* zero = new float(0.0f);
    coord_hash_replace(cost_so_far, coord_copy(start), zero);

    float h_start = heuristic_fn(start, goal, nullptr);
    cost_coord_pq_push(frontier, h_start, coord_copy(start));

    if (visited_logging)
        route_add_visited(result, coord_copy(start));

    int retry = 0;
    coord_t* final = nullptr;

    while (!cost_coord_pq_is_empty(frontier) && retry++ < max_retry) {
        coord_t* current = cost_coord_pq_pop(frontier);
        if (!current) break;

        if (coord_equal(current, goal)) {
            final = coord_copy(current);
            delete current;
            break;
        }

        float* g_ptr = (float*)coord_hash_get(cost_so_far, current);
        float g = g_ptr ? *g_ptr : 0.0f;

        coord_list_t* neighbors = map_clone_neighbors(
            m, current->x, current->y);

        int len = coord_list_length(neighbors);

        for (int i = 0; i < len; ++i) {
            const coord_t* next = coord_list_get(neighbors, i);

            float move_cost = cost_fn(m, current, next, nullptr);
            float new_cost = g + move_cost;

            float* known_cost = (float*)coord_hash_get(cost_so_far, next);
            if (!known_cost || new_cost < *known_cost) {
                coord_hash_replace(
                    cost_so_far, coord_copy(next), new float(new_cost));

                coord_hash_replace(
                    came_from, coord_copy(next), coord_copy(current));

                float h = heuristic_fn(next, goal, nullptr);
                float f = new_cost + h;
                cost_coord_pq_push(frontier, f, coord_copy(next));

                if (visited_logging)
                    route_add_visited(result, coord_copy(next));
            }
        }

        coord_list_free(neighbors);
        coord_free(current);

        int n = cost_coord_pq_length(frontier) - memory_limit;

        if (n > 0) {
            // 제거 우선순위는 높은 f값
            cost_coord_pq_trim_worst(frontier, n);
        }
    }

    if (!final) {
        // 목표를 찾지 못한 경우 가장 낮은 f를 가진 노드로부터 역추적 시도
        final = cost_coord_pq_peek(frontier);
        if (final) final = coord_copy(final);
    }

    if (final) {
        if (route_reconstruct_path(result, came_from, start, final)) {
            route_set_success(result, coord_equal(final, goal));
        } else {
            route_set_success(result, false);
        }
        coord_free(final);
    } else {
        route_set_success(result, false);
    }

    route_set_total_retry_count(result, retry);

    // 메모리 해제
    coord_list_t* keys = coord_hash_keys(cost_so_far);
    int n = coord_list_length(keys);
    for (int i = 0; i < n; ++i) {
        const coord_t* key = coord_list_get(keys, i);
        float* val = (float*)coord_hash_get(cost_so_far, key);
        delete val;
    }
    coord_list_free(keys);

    coord_hash_free(cost_so_far);
    coord_hash_free(came_from);
    cost_coord_pq_free(frontier);

    return result;
}
