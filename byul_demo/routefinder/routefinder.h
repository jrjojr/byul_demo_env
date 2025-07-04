#ifndef ROUTEFINDER_H
#define ROUTEFINDER_H

#include "byul_config.h"
#include "internal/coord.h"
#include "internal/route.h"
#include "internal/map.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BFS 알고리즘으로 경로를 찾습니다.
 *
 * 가장 먼저 발견되는 경로를 반환하며, 최단 경로가 아닐 수 있습니다.
 *
 * @param start 시작 좌표
 * @param goal  목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_bfs(const map_t* m, const coord_t* start, const coord_t* goal);

/**
 * @brief DFS 알고리즘으로 경로를 찾습니다.
 *
 * 깊이 우선으로 탐색하며, 경로 품질은 보장되지 않습니다.
 *
 * @param start 시작 좌표
 * @param goal  목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_dfs(const map_t* m, const coord_t* start, const coord_t* goal);

/**
 * @brief Dijkstra 알고리즘으로 최단 경로를 찾습니다.
 *
 * 비용이 균등하지 않은 맵에서 정확한 최단 경로를 계산합니다.
 *
 * @param start 시작 좌표
 * @param goal  목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_dijkstra(const map_t* m, const coord_t* start, const coord_t* goal);

/**
 * @brief Fast Marching Method로 경로를 찾습니다.
 *
 * 거리 기반 전파 방식을 사용하여 연속적인 확장을 수행합니다.
 *
 * @param start 시작 좌표
 * @param goal  목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_fast_marching(const map_t* m, 
    const coord_t* start, const coord_t* goal);

/**
 * @brief Fringe Search 알고리즘으로 경로를 찾습니다.
 *
 * A*의 효율성과 IDA*의 메모리 절약 구조를 결합한 탐색 방식입니다.
 *
 * @param start 시작 좌표
 * @param goal  목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_fringe_search(const map_t* m, 
    const coord_t* start, const coord_t* goal);

/**
 * @brief Greedy Best-First Search로 경로를 찾습니다.
 *
 * 휴리스틱만을 사용하여 가장 빠른 방향으로만 탐색합니다.
 * 정확한 최단 경로는 보장되지 않지만 속도가 빠릅니다.
 *
 * @param start 시작 좌표
 * @param goal  목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_greedy_best_first(const map_t* m, 
    const coord_t* start, const coord_t* goal);

/**
 * @brief IDA* 알고리즘으로 경로를 찾습니다.
 *
 * DFS 기반으로 반복적으로 깊이를 확장하며, 메모리 사용이 적습니다.
 *
 * @param start 시작 좌표
 * @param goal  목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_ida_star(const map_t* m, const coord_t* start, const coord_t* goal);

/**
 * @brief RTA* 알고리즘으로 경로를 찾습니다.
 *
 * 각 상태에서 제한된 깊이만 탐색하여 즉시 다음 이동을 결정합니다.
 *
 * @param start 시작 좌표
 * @param goal 목표 좌표
 * @param depth_limit 깊이 제한 (1 이상, 일반적으로 3~10)
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_rta_star(const map_t* m, 
    const coord_t* start, const coord_t* goal, int depth_limit);

/**
 * @brief SMA* 알고리즘으로 경로를 찾습니다.
 *
 * 제한된 메모리 환경에서 가능한 최상의 경로를 찾습니다.
 *
 * @param start 시작 좌표
 * @param goal 목표 좌표
 * @param memory_limit 사용할 수 있는 최대 노드 수 (50 이상 권장)
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_sma_star(const map_t* m, 
    const coord_t* start, const coord_t* goal, int memory_limit);

/**
 * @brief Weighted A* 알고리즘으로 경로를 찾습니다.
 *
 * A* 알고리즘의 휴리스틱에 가중치를 적용하여 더 빠르게 목적지에 도달하도록 유도합니다.
 *
 * @param start 시작 좌표
 * @param goal 목표 좌표
 * @param weight 휴리스틱 가중치 값
 *               - 1.0: 일반 A*와 동일 (최단 경로)
 *               - 1.5~2.5: 휴리스틱 우선 경로 (속도 ↑, 정확도 ↓)
 *               - 3.0 이상: 매우 공격적, 최단 경로 아님
 *
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_weighted_astar(const map_t* m, 
    const coord_t* start, const coord_t* goal, float weight);

/**
 * @brief 일반 A* 알고리즘으로 최단 경로를 찾습니다.
 *
 * 비용 함수와 휴리스틱을 함께 사용하여 정확한 최단 경로를 계산합니다.
 *
 * @param start 시작 좌표
 * @param goal 목표 좌표
 * @return 생성된 경로 객체 (route_t*)
 */
BYUL_API route_t* find_astar(const map_t* m, const coord_t* start, const coord_t* goal);

#ifdef __cplusplus
}
#endif

#endif // ROUTEFINDER_H
