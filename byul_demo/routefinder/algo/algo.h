#ifndef ALGO_H
#define ALGO_H

#include "byul_config.h"
#include "internal/core.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DIAGONAL_COST 1.4142135f  // √2 근사값

/*------------------------------------------------------------
 * 함수 포인터
 *------------------------------------------------------------*/

typedef float (*cost_func)(
    const map_t* m, const coord_t* start, const coord_t* goal, void* userdata);

typedef float (*heuristic_func)(
    const coord_t* start, const coord_t* goal, void* userdata);



/** 비용 계산 함수들 */
BYUL_API float default_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata);

BYUL_API float zero_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata);

BYUL_API float diagonal_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata);

/** 유클리드 거리 */
BYUL_API float euclidean_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 맨해튼 거리 */
BYUL_API float manhattan_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 체비셰프 거리 */
BYUL_API float chebyshev_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 옥타일 거리 (8방향 이동 가중치) */
BYUL_API float octile_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 제로 거리 (테스트/그리디) */
BYUL_API float zero_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

BYUL_API float default_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/*------------------------------------------------------------
 * 알고리즘 종류 정의
 *------------------------------------------------------------*/
typedef enum {
    PATH_ALGO_UNKNOWN = 0,

    // 1950s~1960s
    PATH_ALGO_BELLMAN_FORD,            // 1958
    PATH_ALGO_DFS,                     // 1959
    PATH_ALGO_BFS,                     // 1959
    PATH_ALGO_DIJKSTRA,                // 1959
    PATH_ALGO_FLOYD_WARSHALL,          // 1959~
    PATH_ALGO_ASTAR,                   // 1968

    // 1970s
    PATH_ALGO_BIDIRECTIONAL_DIJKSTRA,  // 1971
    PATH_ALGO_BIDIRECTIONAL_ASTAR,     // 1971
    PATH_ALGO_WEIGHTED_ASTAR,          // 1977~
    PATH_ALGO_JOHNSON,                 // 1977
    PATH_ALGO_K_SHORTEST_PATH,         // 1977~
    PATH_ALGO_DIAL,                    // 1969

    // 1980s
    PATH_ALGO_ITERATIVE_DEEPENING,     // 1980
    PATH_ALGO_GREEDY_BEST_FIRST,       // 1985
    PATH_ALGO_IDA_STAR,                // 1985

    // 1990s
    PATH_ALGO_RTA_STAR,                // 1990
    PATH_ALGO_SMA_STAR,                // 1991
    PATH_ALGO_DSTAR,                   // 1994
    PATH_ALGO_FAST_MARCHING,           // 1996
    PATH_ALGO_ANT_COLONY,              // 1996
    PATH_ALGO_FRINGE_SEARCH,           // 1997

    // 2000s
    PATH_ALGO_FOCAL_SEARCH,            // 2001
    PATH_ALGO_DSTAR_LITE,              // 2002
    PATH_ALGO_LPA_STAR,                // 2004
    PATH_ALGO_HPA_STAR,                // 2004
    PATH_ALGO_ALT,                     // 2005
    PATH_ALGO_ANY_ANGLE_ASTAR,         // 2005~
    PATH_ALGO_HCA_STAR,                // 2005
    PATH_ALGO_RTAA_STAR,               // 2006
    PATH_ALGO_THETA_STAR,              // 2007
    PATH_ALGO_CONTRACTION_HIERARCHIES,// 2008

    // 2010s
    PATH_ALGO_LAZY_THETA_STAR,         // 2010
    PATH_ALGO_JUMP_POINT_SEARCH,       // 2011
    PATH_ALGO_SIPP,                    // 2011
    PATH_ALGO_JPS_PLUS,                // 2012
    PATH_ALGO_EPEA_STAR,               // 2012
    PATH_ALGO_MHA_STAR,                // 2012
    PATH_ALGO_ANYA,                    // 2013

    // 특수 목적 / 확장형
    PATH_ALGO_DAG_SP,                  // 1960s (DAG 최단경로 O(V+E))
    PATH_ALGO_MULTI_SOURCE_BFS,        // 2000s (복수 시작점 BFS)
    PATH_ALGO_MCTS                     // 2006
} route_algotype_t;

typedef enum {
    FRONTIER_QUEUE,
    FRONTIER_PRIORQ
} frontier_type_t;

BYUL_API const char* get_algo_name(route_algotype_t pa);

#ifdef __cplusplus
}
#endif

#endif // ALGO_H
