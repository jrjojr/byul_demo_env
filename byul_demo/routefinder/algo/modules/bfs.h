#ifndef BFS_H
#define BFS_H

#include "byul_config.h"
#include "core.h"
#include "internal/coord.h"
#include "internal/map.h"
#include "internal/route.h"
#include "internal/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BFS(Breadth-First Search) 알고리즘을 이용해 경로를 탐색합니다.
 *
 * BFS는 너비 우선 탐색 방식으로, 각 노드를 동일한 비용으로 간주하여
 * 가장 가까운 노드부터 탐색하는 방식입니다. 가중치나 휴리스틱을 사용하지 않으며,
 * 최단 이동 횟수를 기준으로 한 경로를 보장합니다 (모든 간선의 비용이 동일할 때).
 *
 * 이 구현은 @c algo 컨텍스트를 기반으로 하며, 
 * 내부적으로 단순 큐(FIFO)를 사용하여 탐색 순서를 유지합니다.
 *
 * @param al    알고리즘 컨텍스트. 
 *              @c algo_new_default() 또는 
 *              @c algo_new_full() 로 생성되어야 합니다.
 * 
 * @param start  시작 좌표
 * @param goal    도착 좌표
 * @return route 경로 객체. 
 *              @c route_get_success(p) 가 TRUE이면 탐색에 성공한 것입니다.
 *
 * @usage 예시:
 * @code
 * algo al = algo_new_default(10, 10, PATH_ALGO_BFS, TRUE);
 *
 * // 수평 벽 생성 (3번째 행)
 * for (int x = 1; x < 10; x++)
 *     map_block_coord(al->m, x, 3);
 *
 * coord start = coord_new_full(0, 0);
 * coord goal   = coord_new_full(9, 9);
 *
 * route p = bfs_find(al, start, goal);
 * if (route_get_success(p)) {
 *     g_print("[TEST] map_print_ascii_with_visited_count\n");
 *     map_print_ascii_with_visited_count(al->m, p, start, goal);
 * }
 *
 * route_free(p);
 * coord_free(start);
 * coord_free(goal);
 * algo_free(al);
 * @endcode
 *
 * @note
 * BFS는 최단 경로를 찾을 수는 있지만, 실제 거리(가중치)를 고려하지 않으므로
 * 지형에 따른 이동 비용이 존재할 경우 부정확한 결과를 반환할 수 있습니다.
 *
 * @see algo_new_default(), 
 *      map_block_coord(), 
 *      map_print_ascii_with_visited_count(), 
 *      route_get_success()
 */
BYUL_API route bfs_find(const algo al, const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // BFS_H
