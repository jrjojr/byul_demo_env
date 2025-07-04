#include "internal/coord_finder.h"
#include "internal/pqueue.h"
#include "internal/core.h"

/**
 * @brief 내부적으로 BFS를 수행하여 도달 가능한 가장 가까운 좌표를 탐색한다.
 */
gboolean find_goal_bfs(const coord_t* start,
                       is_reeachable_func is_reachable_fn,
                       gpointer user_data,
                       gint max_range,
                       coord_t* *out_result)
{
    if (!out_result || !is_reachable_fn || max_range <= 0 || max_range > MAX_RANGE_LIMIT)
        return FALSE;

    GQueue *queue = g_queue_new();
    GHashTable *visited = g_hash_table_new_full(
        (GHashFunc) coord_hash, (GEqualFunc)coord_equal, 
        (GDestroyNotify) coord_free, NULL);

    g_queue_push_tail(queue, coord_copy(start));
    g_hash_table_insert(visited, coord_copy(start), GINT_TO_POINTER(1));

    const coord_t offsets[4] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    while (!g_queue_is_empty(queue)) {
        coord_t* cur = (coord_t*) g_queue_pop_head(queue);

        if (is_reachable_fn(cur, user_data)) {
            *out_result = coord_copy(cur);
            coord_free(cur);
            g_queue_free_full(queue, (GDestroyNotify) coord_free);
            g_hash_table_destroy(visited);
            return TRUE;
        }

        for (int i = 0; i < 4; ++i) {
            coord_t next = {
                .x = cur->x + offsets[i].x,
                .y = cur->y + offsets[i].y
            };

            if (coord_distance(start, &next) > max_range)
                continue;

            if (g_hash_table_contains(visited, &next))
                continue;

            g_queue_push_tail(queue, coord_copy(&next));
            g_hash_table_insert(visited, coord_copy(&next), GINT_TO_POINTER(1));
        }

        coord_free(cur);
    }

    g_queue_free_full(queue, (GDestroyNotify)coord_free);
    g_hash_table_destroy(visited);

    // *out_result = (coord_t){ .x = -1, .y = -1 };
    *out_result  = coord_new_full(-1, -1);
    return FALSE;
}

gint astar_node_compare(gconstpointer a, gconstpointer b, gpointer user_data) {
    const astar_node na = (const astar_node) a;
    const astar_node nb = (const astar_node) b;
    gint fa = na->cost + na->heuristic;
    gint fb = nb->cost + nb->heuristic;
    return fa - fb;
}

/**
 * @brief GPriorityQueue 기반 A* 방식으로 가장 가까운 reachable 좌표를 탐색
 */
gboolean find_goal_astar(const coord_t* start,
                          is_reeachable_func is_reachable_fn,
                          gpointer user_data,
                          gint max_range,
                          coord_t* *out_result)
{
    if (!out_result || !is_reachable_fn || max_range <= 0)
        return FALSE;

    GHashTable *visited = g_hash_table_new_full(
        (GHashFunc) coord_hash, (GEqualFunc) coord_equal,
        (GDestroyNotify) coord_free, NULL);

    pqueue open_list = pqueue_new_full((GCompareDataFunc) compare_float,
        NULL, g_free, g_free);

    astar_node start_node = (astar_node) g_malloc(sizeof(astar_node_t));
    start_node->m_coord_t = *start;
    start_node->cost = 0;
    start_node->heuristic = 0;
    gfloat priority = (gfloat)(start_node->cost + start_node->heuristic);
    pqueue_push(open_list, &priority, sizeof(gfloat), 
            start_node, sizeof(astar_node_t));

    g_free(start_node);

    g_hash_table_insert(visited, coord_copy(start), GINT_TO_POINTER(1));

    const coord_t offsets[4] = {
        {0, -1}, {1, 0}, {0, 1}, {-1, 0}
    };

    while (!pqueue_is_empty(open_list)) {
        astar_node cur_node = (astar_node) pqueue_pop(open_list);
        coord_t cur = cur_node->m_coord_t;

        if (is_reachable_fn(&cur, user_data)) {
            *out_result = coord_copy(&cur);
            g_free(cur_node);
            pqueue_free(open_list);
            g_hash_table_destroy(visited);
            return TRUE;
        }

        for (int i = 0; i < 4; ++i) {
            coord_t next = {
                .x = cur.x + offsets[i].x,
                .y = cur.y + offsets[i].y
            };

            if (coord_distance(start, &next) > max_range)
                continue;

            if (g_hash_table_contains(visited, &next))
                continue;

            astar_node next_node = (astar_node) g_malloc(sizeof(astar_node_t));
            next_node->m_coord_t.x = next.x;
            next_node->m_coord_t.y = next.y;
            
            next_node->cost = cur_node->cost + 1;
            next_node->heuristic = coord_distance(start, &next);

            gfloat next_priority = (gfloat)(next_node->cost + next_node->heuristic);
            // pqueue_push(open_list, next_node, sizeof(astar_node_t), &next_priority, sizeof(gfloat));
            pqueue_push(open_list, &next_priority, sizeof(gfloat), 
                next_node, sizeof(astar_node_t));
            g_hash_table_insert(visited, coord_copy(&next), GINT_TO_POINTER(1));

            g_free(next_node);
        }

        g_free(cur_node);
    }

    pqueue_free(open_list);
    g_hash_table_destroy(visited);

    *out_result = coord_new_full(-1, -1);
    return FALSE;
}


