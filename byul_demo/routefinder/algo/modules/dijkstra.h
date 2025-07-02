#ifndef DIJKSTRA_H
#define DIJKSTRA_H

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
 * @brief Dijkstra 알고리즘을 사용하여 최단 경로를 탐색합니다.
 *
 * Dijkstra 알고리즘은 휴리스틱을 사용하지 않고, 
 * 시작 지점에서 목표 지점까지의 실제 누적 비용(g)만을 기준으로
 * 모든 경로를 탐색하여 최단 경로를 찾습니다.
 *
 * A*와 달리 f-score에 휴리스틱 항(h)을 포함하지 않으며,
 * 따라서 항상 최단 경로를 찾지만 탐색 노드 수는 많아질 수 있습니다.
 *
 * 이 함수는 @c algo 구조체를 기반으로 하며,
 * 내부적으로 비용 함수(@c cost_fn), 방문 추적, 
 * 우선순위 큐(@c frontier), 방문 로깅 등을 활용합니다.
 *
 * @param al     알고리즘 컨텍스트 (algo 구조체). 반드시 초기화되어 있어야 합니다.
 * @param start   시작 좌표
 * @param goal     도착 좌표
 * @return route  경로 객체. 탐색에 성공한 경우 
 *                  @c route_get_success(p) 가 TRUE를 반환합니다.
 *
 * @usage 예시:
 * @code
 * coord start = coord_new_full(0, 0);
 * coord goal   = coord_new_full(9, 9);
 *
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_DIJKSTRA,
 *     default_cost,
 *     default_heuristic,  // 휴리스틱은 사용되지 않음
 *     NULL,   // userdata
 *     NULL,   // algo_specific
 *     TRUE    // 방문 순서 로깅 활성화
 * );
 *
 * // 장애물 설치 (열 한가운데를 막음)
 * for (int y = 1; y < 10; y++)
 *     map_block_coord(al->m, 5, y);
 *
 * route p = dijkstra_find(al, start, goal);
 * if (route_get_success(p)) {
 *     map_print_ascii_with_visited_count(al->m, p, start, goal);
 * }
 *
 * route_free(p);
 * coord_free(start);
 * coord_free(goal);
 * algo_free(al);
 * @endcode
 *
 * @see algo_new_full(), 
 *      route_get_success(), 
 *      map_print_ascii_with_visited_count()
 */
BYUL_API route dijkstra_find(const algo al, const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // DIJKSTRA_H
