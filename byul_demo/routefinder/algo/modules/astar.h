#ifndef ASTAR_H
#define ASTAR_H

#include "byul_config.h"
#include "core.h"

#include "internal/algo.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A* 알고리즘을 이용해 최단 경로를 탐색합니다.
 *
 * A* 알고리즘은 실제 거리(g)와 휴리스틱 거리(h)의 합을 기준으로 경로를 확장하는 
 * 휴리스틱 기반의 최적화 탐색 알고리즘입니다. 아래와 같은 방식으로 f-score를 계산합니다:
 *
 *     f(n) = g(n) + h(n)
 *
 * 이 함수는 내부적으로 algo 구조체의 cost 함수와 heuristic 함수를 사용하며,
 * 경로 탐색 중 방문한 좌표들은 route에 로깅됩니다.
 *
 * @param al     알고리즘 컨텍스트 (algo 구조체). 반드시 초기화된 상태여야 합니다.
 * @param start   시작 좌표
 * @param goal     도착 좌표
 * @return route  탐색 결과 경로 객체. 실패 시 route_get_success()는 FALSE를 반환합니다.
 *
 * @usage 예시:
 * @code
 * coord start = coord_new_full(0, 0);
 * coord goal   = coord_new_full(9, 9);
 *
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_ASTAR,
 *     default_cost,
 *     default_heuristic,
 *     NULL,   // userdata
 *     NULL,   // algo_specific
 *     TRUE    // 방문 순서 로깅 활성화
 * );
 *
 * // 경로를 막는 장애물 추가
 * for (int y = 1; y < 10; y++)
 *     map_block_coord(al->m, 5, y);
 *
 * route p = astar_find(al, start, goal);
 *
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
BYUL_API route astar_find(const algo al, const coord start, const coord goal);


#ifdef __cplusplus
}
#endif

#endif // ASTAR_H
