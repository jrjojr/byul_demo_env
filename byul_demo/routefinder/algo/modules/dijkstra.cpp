#include "internal/dijkstra.h"
#include "internal/cost_coord_pq.h"
#include "internal/map.h"
#include "coord.hpp"
#include "internal/coord_list.h"
#include "internal/coord_hash.h"
#include "internal/route.h"

route_t* find_dijkstra(const map_t* m,
    const coord_t* start, const coord_t* goal, cost_func cost_fn,
    int max_retry, bool visited_logging) {

    if (!m || !start || !goal || max_retry <= 0) return nullptr;

    if(!cost_fn) cost_fn = default_cost;

    cost_coord_pq_t* pq = cost_coord_pq_new();
    coord_hash_t* cost_so_far = coord_hash_new();
    coord_hash_t* came_from = coord_hash_new();
    route_t* result = route_new();

    if (visited_logging)
        route_add_visited(result, coord_copy(start));

    float* zero = new float(0.0f);
    coord_hash_replace(cost_so_far, coord_copy(start), zero);
    cost_coord_pq_push(pq, 0.0f, coord_copy(start));

    bool found = false;
    coord_t* final = nullptr;
    int retry = 0;

    while (!cost_coord_pq_is_empty(pq) && retry++ < max_retry) {
        coord_t* current = cost_coord_pq_pop(pq);

        if (coord_equal(current, goal)) {
            found = true;
            final = coord_copy(current);
            delete current;
            break;
        }

        float* current_cost_ptr = (float*)coord_hash_get(cost_so_far, current);
        float current_cost = current_cost_ptr ? *current_cost_ptr : 0.0f;

        coord_list_t* neighbors = map_clone_neighbors(
            m, current->x, current->y);

        int len = coord_list_length(neighbors);
        for (int i = 0; i < len; ++i) {
            const coord_t* next = coord_list_get(neighbors, i);
            float move_cost = cost_fn(m, current, next, nullptr);
            float new_cost = current_cost + move_cost;

            float* known_cost = (float*)coord_hash_get(cost_so_far, next);
            if (!known_cost || new_cost < *known_cost) {
                float* new_cost_ptr = new float(new_cost);
                coord_hash_replace(cost_so_far, coord_copy(next), new_cost_ptr);
                cost_coord_pq_push(pq, new_cost, coord_copy(next));
                coord_hash_replace(
                    came_from, coord_copy(next), coord_copy(current));

                if (visited_logging)
                    route_add_visited(result, coord_copy(next));
            }
        }

        coord_list_free(neighbors);
        // if (!final) final = coord_copy(current);  // 최후 탐색 위치
        if (final) coord_free(final);
        final = coord_copy(current);
        delete current;
    }

    // if (final) {
        if (route_reconstruct_path(result, came_from, start, final)) {
            route_set_success(result, found);
        } else {
            route_set_success(result, false);
        }
        delete final;
    // } else {
    //     route_set_success(result, false);
    // }

    // 해제
    coord_list_t* cost_keys = coord_hash_keys(cost_so_far);
    int cost_len = coord_list_length(cost_keys);
    for (int i = 0; i < cost_len; ++i) {
        const coord_t* key = coord_list_get(cost_keys, i);
        float* val = (float*)coord_hash_get(cost_so_far, key);
        delete val;
    }
    coord_list_free(cost_keys);

    cost_coord_pq_free(pq);
    coord_hash_free(cost_so_far);
    coord_hash_free(came_from);

    route_set_total_retry_count(result, retry);
    return result;
}
