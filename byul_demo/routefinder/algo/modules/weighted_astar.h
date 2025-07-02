#ifndef WEIGHTED_ASTAR_H
#define WEIGHTED_ASTAR_H

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
 * @brief Weighted A* 알고리즘 설정 구조체
 *
 * @param weight 휴리스틱에 곱해지는 가중치 계수 (1.0 이상)
 */
typedef struct s_weighted_astar_config {
    gfloat weight;
} weighted_astar_config_t;

typedef weighted_astar_config_t* weighted_astar_config;

// 가중치 계수 1.0으로 생성
BYUL_API weighted_astar_config weighted_astar_config_new();

/**
 * @brief Weighted A* 알고리즘을 위한 구성 객체를 생성합니다.
 *
 * Weighted A*는 전통적인 A* 탐색의 휴리스틱 영향력을 조절하는 확장 알고리즘입니다.
 * 이 함수는 지정한 가중치(weight)를 기반으로 f-score 계산식을 다음과 같이 구성합니다:
 *
 *     f(n) = g(n) + weight × h(n)
 *
 * - g(n): 시작점부터 현재 노드까지의 실제 누적 비용
 * - h(n): 현재 노드에서 목표 지점까지의 휴리스틱 추정값
 * - weight: h(n)의 영향력을 조절하는 상수
 *
 * @param weight
 *   휴리스틱 계수.
 *   - `weight = 1.0`이면 일반적인 A*와 동일합니다.
 *   - `weight < 1.0`이면 더 정확한 경로를 찾지만 더 많은 노드를 확장합니다. 
 *      (Underweighted A*)
 * 
 *   - `weight > 1.0`이면 더 빠르지만 경로 품질은 떨어집니다.
 *   - `weight = 0.0`이면 휴리스틱이 완전히 무시되어 Dijkstra와 동일해집니다.
 *   - `weight >> 1.0` (예: 100 이상)이면 실제 거리를 무시하고 
 *      Greedy Best-First Search에 가까워집니다.
 *
 * @return weighted_astar_config
 *   생성된 구성 객체. 실패 시 NULL을 반환합니다.
 *
 * @note
 *   음수 가중치는 논리적으로 의미가 없으며, 내부적으로 1.0으로 대체하는 것이 안전합니다.
 */
BYUL_API weighted_astar_config weighted_astar_config_new_full(gfloat weight);

BYUL_API void weighted_astar_config_free(weighted_astar_config cfg);

/**
 * @brief Weighted A* 알고리즘으로 경로를 탐색합니다.
 *
 * Weighted A*는 전통적인 A* 알고리즘에서 휴리스틱 함수의 영향력을 조절할 수 있도록
 * 가중치(weight)를 부여한 확장 알고리즘입니다.
 *
 * f-score 계산은 다음과 같습니다:
 *     f(n) = g(n) + weight × h(n)
 *
 * 여기서 g(n)은 시작점부터 현재 노드까지의 실제 누적 비용,
 * h(n)은 현재 노드에서 목표까지의 휴리스틱 거리이며,
 * weight는 @c algo->algo_specific 에 설정된 
 * @c weighted_astar_config 에서 불러옵니다.
 *
 * @param al     알고리즘 컨텍스트 (algo 구조체). 
 *              내부에 weighted_astar_config가 연결되어 있어야 합니다.
 * @param start   시작 좌표
 * @param goal     도착 좌표
 * @return route  경로 결과. 실패 시 route 구조체의 success 필드가 FALSE로 설정됩니다.
 *
 * @see weighted_astar_config_new_full()
 *
 * @usage 예시:
 * @code
 * coord start = coord_new_full(0, 0);
 * coord goal   = coord_new_full(9, 9);
 * weighted_astar_config cfg = weighted_astar_config_new_full(2.0f);
 *
 * algo al = algo_new_full(
 *     10, 10,
 *     MAP_NEIGHBOR_8,
 *     PATH_ALGO_WEIGHTED_ASTAR, 
 *     default_cost,
 *     default_heuristic,
 *     NULL,    // userdata
 *     cfg,     // algo_specific → weight 설정 포함
 *     TRUE     // visited 로깅
 * );
 *
 * route p = weighted_astar_find(al, start, goal);
 * if (route_get_success(p)) {
 *     map_print_ascii_with_route(al->m, p, start, goal);
 * }
 *
 * route_free(p);
 * coord_free(start);
 * coord_free(goal);
 * algo_free(al);
 * weighted_astar_config_free(cfg);
 * @endcode
 */
BYUL_API route weighted_astar_find(const algo al, 
    const coord start, const coord goal);

#ifdef __cplusplus
}
#endif

#endif // WEIGHTED_ASTAR_H
