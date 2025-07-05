#ifndef ALGO_H
#define ALGO_H

#include "internal/algo_common.h"

// 모든 알고리즘 모듈 포함
#include "internal/astar.h"
#include "internal/bfs.h"
#include "internal/dfs.h"
#include "internal/dijkstra.h"
#include "internal/fast_marching.h"
#include "internal/fringe_search.h"
#include "internal/greedy_best_first.h"
#include "internal/ida_star.h"
#include "internal/rta_star.h"
#include "internal/sma_star.h"
#include "internal/weighted_astar.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum e_route_algotype{
    ROUTE_ALGO_UNKNOWN = 0,

    // 1950s~1960s
    ROUTE_ALGO_BELLMAN_FORD,            // 1958
    ROUTE_ALGO_DFS,                     // 1959
    ROUTE_ALGO_BFS,                     // 1959
    ROUTE_ALGO_DIJKSTRA,                // 1959
    ROUTE_ALGO_FLOYD_WARSHALL,          // 1959~
    ROUTE_ALGO_ASTAR,                   // 1968

    // 1970s
    ROUTE_ALGO_BIDIRECTIONAL_DIJKSTRA,  // 1971
    ROUTE_ALGO_BIDIRECTIONAL_ASTAR,     // 1971
    ROUTE_ALGO_WEIGHTED_ASTAR,          // 1977~
    ROUTE_ALGO_JOHNSON,                 // 1977
    ROUTE_ALGO_K_SHORTEST_PATH,         // 1977~
    ROUTE_ALGO_DIAL,                    // 1969

    // 1980s
    ROUTE_ALGO_ITERATIVE_DEEPENING,     // 1980
    ROUTE_ALGO_GREEDY_BEST_FIRST,       // 1985
    ROUTE_ALGO_IDA_STAR,                // 1985

    // 1990s
    ROUTE_ALGO_RTA_STAR,                // 1990
    ROUTE_ALGO_SMA_STAR,                // 1991
    ROUTE_ALGO_DSTAR,                   // 1994
    ROUTE_ALGO_FAST_MARCHING,           // 1996
    ROUTE_ALGO_ANT_COLONY,              // 1996
    ROUTE_ALGO_FRINGE_SEARCH,           // 1997

    // 2000s
    ROUTE_ALGO_FOCAL_SEARCH,            // 2001
    ROUTE_ALGO_DSTAR_LITE,              // 2002
    ROUTE_ALGO_LPA_STAR,                // 2004
    ROUTE_ALGO_HPA_STAR,                // 2004
    ROUTE_ALGO_ALT,                     // 2005
    ROUTE_ALGO_ANY_ANGLE_ASTAR,         // 2005~
    ROUTE_ALGO_HCA_STAR,                // 2005
    ROUTE_ALGO_RTAA_STAR,               // 2006
    ROUTE_ALGO_THETA_STAR,              // 2007
    ROUTE_ALGO_CONTRACTION_HIERARCHIES,// 2008

    // 2010s
    ROUTE_ALGO_LAZY_THETA_STAR,         // 2010
    ROUTE_ALGO_JUMP_POINT_SEARCH,       // 2011
    ROUTE_ALGO_SIPP,                    // 2011
    ROUTE_ALGO_JPS_PLUS,                // 2012
    ROUTE_ALGO_EPEA_STAR,               // 2012
    ROUTE_ALGO_MHA_STAR,                // 2012
    ROUTE_ALGO_ANYA,                    // 2013

    // 특수 목적 / 확장형
    ROUTE_ALGO_DAG_SP,                  // 1960s (DAG 최단경로 O(V+E))
    ROUTE_ALGO_MULTI_SOURCE_BFS,        // 2000s (복수 시작점 BFS)
    ROUTE_ALGO_MCTS                     // 2006
} route_algotype_t;

BYUL_API const char* get_algo_name(route_algotype_t pa);

/** 
 * @brief 정적 길찾기 설정 구조체
 */
typedef struct s_algo {
    map_t* map;                        ///< 경로를 탐색할 지도
    route_algotype_t type;
    coord_t* start;                     ///< 시작 좌표
    coord_t* goal;                      ///< 도착 좌표
    cost_func cost_fn;                ///< 비용 함수
    heuristic_func heuristic_fn;      ///< 휴리스틱 함수
    int max_retry;                    ///< 최대 반복 횟수
    bool visited_logging;             ///< 방문한 노드 로깅 여부
    void* userdata;                   ///< 사용자 정의 데이터
} algo_t;

/**
 * @brief 기본 설정으로 algo_t 구조체를 생성합니다.
 *
 * 이 함수는 ROUTE_ALGO_ASTAR를 기본 알고리즘으로 설정하고,
 * 다음과 같은 기본값을 포함한 algo_t 객체를 생성합니다:
 * - cost 함수: default_cost
 * - 휴리스틱 함수: euclidean_heuristic
 * - 최대 반복 횟수: 10000
 * - 방문 노드 로깅: false
 *
 * @return 초기화된 algo_t 포인터 (heap에 생성되며, 사용 후 algo_free로 해제해야 함)
 */
BYUL_API algo_t* algo_new(map_t* map);

BYUL_API algo_t* algo_new_full(map_t* map, 
    route_algotype_t type, 
    coord_t* start, coord_t* goal,
    cost_func cost_fn, heuristic_func heuristic_fn,
    int max_retry, bool visited_logging, void* userdata);

BYUL_API void algo_free(algo_t* a);

BYUL_API algo_t* algo_copy(const algo_t* src);

/**
 * @brief 설정값 세터/게터
 */
BYUL_API void algo_set_map(algo_t* a, map_t* map);
BYUL_API void algo_set_start(algo_t* a, coord_t* start);
BYUL_API void algo_set_goal(algo_t* a, coord_t* goal);

BYUL_API map_t* algo_get_map(const algo_t* a);
BYUL_API coord_t* algo_get_start(const algo_t* a);
BYUL_API coord_t* algo_get_goal(const algo_t* a);

BYUL_API void algo_set_userdata(algo_t* a, void* userdata);
BYUL_API void* algo_get_userdata(algo_t* a);

/**
 * @brief 설정값 기본화 및 검증
 */
BYUL_API void algo_clear(algo_t* a);

/**
 * @brief algo_t 구조체의 기본값을 설정합니다.
 *
 * - cost 함수는 default_cost,
 * - 휴리스틱 함수는 euclidean_heuristic,
 * - 최대 반복 횟수는 10000,
 * - visited_logging은 false로 초기화됩니다.
 *
 * @param a 기본값을 설정할 algo_t 포인터
 */
BYUL_API void algo_set_defaults(algo_t* a);

BYUL_API bool algo_is_valid(const algo_t* a);
BYUL_API void algo_print(const algo_t* a);

/**
 * @brief 정적 길찾기 실행 (알고리즘 유형 분기 포함)
 */
BYUL_API route_t* algo_find(const algo_t* a, route_algotype_t type);

/**
 * @brief 알고리즘별 직접 실행 함수 (정적 길찾기 전용)
 */
BYUL_API route_t* algo_find_astar(const algo_t* a);
BYUL_API route_t* algo_find_bfs(const algo_t* a);
BYUL_API route_t* algo_find_dfs(const algo_t* a);
BYUL_API route_t* algo_find_dijkstra(const algo_t* a);

/**
 * @brief Fringe Search 알고리즘을 실행합니다.
 *
 * 이 알고리즘은 fringe threshold를 넘기며 탐색하는 방식으로,
 * 탐색 효율을 높이기 위해 사용자 정의 매개변수 delta_epsilon을 사용합니다.
 *
 * @param a 실행 설정이 포함된 algo_t 포인터
 *          - userdata는 float* 타입이며, fringe 확장 임계값인 
 *                  delta_epsilon을 가리켜야 합니다.
 *          - 추천값: 0.1 ~ 0.5 (기본값 없음, 사용자 설정 필요)
 *
 * @return 계산된 경로(route_t*) 또는 실패 시 NULL
 */
BYUL_API route_t* algo_find_fringe_search(const algo_t* a);

BYUL_API route_t* algo_find_greedy_best_first(const algo_t* a);
BYUL_API route_t* algo_find_ida_star(const algo_t* a);

/**
 * @brief Real-Time A* (RTA*) 알고리즘을 실행합니다.
 *
 * 이 알고리즘은 제한된 탐색 깊이 내에서만 탐색을 진행하고
 * 실시간 반응성을 확보합니다.
 *
 * @param a 실행 설정이 포함된 algo_t 포인터
 *          - userdata는 int* 타입이며, 탐색 깊이 제한(depth_limit)을 가리켜야 합니다.
 *          - 추천값: 3 ~ 10 (높을수록 정확하지만 느려짐)
 *
 * @return 계산된 경로(route_t*) 또는 실패 시 NULL
 */
BYUL_API route_t* algo_find_rta_star(const algo_t* a);

BYUL_API route_t* algo_find_sma_star(const algo_t* a);

/**
 * @brief Weighted A* 알고리즘을 실행합니다.
 *
 * 이 알고리즘은 A*의 휴리스틱에 가중치를 적용해
 * 더 빠른 경로 계산을 유도합니다.
 *
 * @param a 실행 설정이 포함된 algo_t 포인터
 *          - userdata는 float* 타입이며, 휴리스틱 가중치(weight)를 가리켜야 합니다.
 *          - 추천값: 1.0 (기본 A*), 1.2 ~ 2.5 (속도 향상), 5.0 이상은 부정확할 수 있음
 *
 * @return 계산된 경로(route_t*) 또는 실패 시 NULL
 */
BYUL_API route_t* algo_find_weighted_astar(const algo_t* a);

BYUL_API route_t* algo_find_fast_marching(const algo_t* a);

#ifdef __cplusplus
}
#endif

#endif // ALGO_H