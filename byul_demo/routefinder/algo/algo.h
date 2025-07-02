#ifndef ALGO_H
#define ALGO_H

#include "byul_config.h"
#include "core.h"
#include "internal/map.h"
#include "internal/coord.h"
#include "internal/route.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DIAGONAL_COST 1.4142135f  // √2 근사값

/** 비용 계산 함수들 */
BYUL_API gfloat default_cost(const map m, 
    const coord start, const coord goal, gpointer userdata);

BYUL_API gfloat zero_cost(const map m, 
    const coord start, const coord goal, gpointer userdata);

BYUL_API gfloat terrain_cost(const map m, 
    const coord start, const coord goal, gpointer userdata);

BYUL_API gfloat diagonal_cost(const map m, 
    const coord start, const coord goal, gpointer userdata);

BYUL_API gfloat height_cost(const map m, 
    const coord start, const coord goal, gpointer userdata);    

/** 유클리드 거리 */
BYUL_API gfloat euclidean_heuristic(const coord start, const coord goal, 
    gpointer userdata);

/** 맨해튼 거리 */
BYUL_API gfloat manhattan_heuristic(const coord start, const coord goal, 
    gpointer userdata);

/** 체비셰프 거리 */
BYUL_API gfloat chebyshev_heuristic(const coord start, const coord goal, 
    gpointer userdata);

/** 옥타일 거리 (8방향 이동 가중치) */
BYUL_API gfloat octile_heuristic(const coord start, const coord goal, 
    gpointer userdata);

/** 제로 거리 (테스트/그리디) */
BYUL_API gfloat zero_heuristic(const coord start, const coord goal, 
    gpointer userdata);

BYUL_API gfloat default_heuristic(const coord start, const coord goal, 
    gpointer userdata);


/*------------------------------------------------------------
 * 알고리즘 종류 정의
 *------------------------------------------------------------*/
typedef enum {
    PATH_ALGO_UNKNOWN = 0,

    // 1950~1960s
    PATH_ALGO_BFS,                   // 1959
    PATH_ALGO_DFS,                   // 1959
    PATH_ALGO_DIJKSTRA,              // 1959
    PATH_ALGO_ASTAR,                 // 1968

    // 1970s~
    PATH_ALGO_WEIGHTED_ASTAR,        // 1975~

    // 1980s
    PATH_ALGO_GREEDY_BEST_FIRST,     // 1985
    PATH_ALGO_IDA_STAR,              // 1985

    // 1990s
    PATH_ALGO_RTA_STAR,              // 1990
    PATH_ALGO_SMA_STAR,              // 1991
    PATH_ALGO_FAST_MARCHING,         // 1996
    PATH_ALGO_FRINGE_SEARCH,         // 1997

    // 2000s
    PATH_ALGO_DSTAR_LITE,            // 2002
    PATH_ALGO_DYNAMIC_ASTAR,         // 2004
    PATH_ALGO_LPA_STAR,              // 2004
    PATH_ALGO_HPA_STAR,              // 2004
    PATH_ALGO_ANY_ANGLE_ASTAR,       // 2005
    PATH_ALGO_ALT,                   // 2005
    PATH_ALGO_THETA_STAR,            // 2007
    PATH_ALGO_LAZY_THETA_STAR,       // 2010
    PATH_ALGO_JUMP_POINT_SEARCH,     // 2011
    PATH_ALGO_JPS_PLUS,              // 2012

    // 확장 알고리즘
    PATH_ALGO_BIDIRECTIONAL_ASTAR    // 1989 (후순위 유지)
} route_algotype_t;

typedef enum {
    FRONTIER_QUEUE,
    FRONTIER_PRIORQ
} frontier_type_t;

/*------------------------------------------------------------
 * 함수 포인터 타입
 *------------------------------------------------------------*/

typedef gfloat (*cost_func)(
    const map m, const coord start, const coord goal, gpointer userdata);

typedef gfloat (*heuristic_func)(
    const coord start, const coord goal, gpointer userdata);

typedef struct s_algo* algo;
typedef route (*algo_find_func)(
    const algo al, const coord start, const coord goal);

/*------------------------------------------------------------
 * 알고리즘 실행 컨텍스트 구조체
 *------------------------------------------------------------*/
typedef struct s_algo {
    map m;                               // 맵
    route_algotype_t algotype;
    frontier_type_t frontier_type;
    algo_find_func algo_find_fn;
    cost_func cost_fn;                   // 비용 함수
    heuristic_func heuristic_fn;         // 휴리스틱 함수
    gpointer userdata;                   // 사용자 정의 데이터
    gpointer algo_specific;              // 알고리즘 전용 구조체

    coord start;  // ← 추가
    coord goal;    // ← 추가    

    GHashTable* visited;                 // 방문 여부 (set)
    GHashTable* came_from;              // 현재 좌표가 어디에서 왔나 (dict)
    GHashTable* cost_so_far;            // 현재 좌표의 비용 (dict)
    gpointer        frontier;           // 탐색 프론티어 (큐/스택/PQ 등)

    gboolean visited_logging;  // route에 방문기록을 로깅
} algo_t;

/*------------------------------------------------------------
 * 생성 및 해제
 *------------------------------------------------------------*/

// 10 x 10, 8방향, bfs, visited_logging = false
BYUL_API algo algo_new(void);

// 8방향, 유클리드 거리, 비용 1.0 이 기본이다.
BYUL_API algo algo_new_default(
    gint width,                        // 맵 가로 크기
    gint height,                       // 맵 세로 크기
    route_algotype_t algotype,    
    gboolean visited_logging    // 디버깅용 방문 순서 로깅 여부
);

BYUL_API algo algo_new_full(
    gint width,                        // 맵 가로 크기
    gint height,                       // 맵 세로 크기
    map_neighbor_mode_t mode,         // 맵 이웃 연결 방식 (4방향/8방향 등)
    route_algotype_t algotype,    
    cost_func cost_fn,                   // 비용 함수 (NULL이면 기본 1.0)
    heuristic_func heuristic_fn,         // 휴리스틱 함수 (NULL이면 유클리드 거리)
    gpointer userdata,                // 사용자 정의 포인터
    gpointer algo_specific,           // 알고리즘 전용 구조체
    gboolean visited_logging    // 디버깅용 방문 순서 로깅 여부
);

BYUL_API void algo_free(algo al);

BYUL_API void algo_free_full(algo al, GDestroyNotify specific_free_func);

/*------------------------------------------------------------
 * 핵심 함수
 *------------------------------------------------------------*/

// 이걸 위해 algo.h가 존재한다.
// 실제 길찾기 알고를 실행하고 결과를 반환한다.
BYUL_API route algo_find(const algo al, const coord start, const coord goal);

/*------------------------------------------------------------
 * 설정자 (Setter)
 *------------------------------------------------------------*/

BYUL_API void algo_set_cost_func(algo al, cost_func func);
BYUL_API void algo_set_heuristic_func(algo al, heuristic_func func);
BYUL_API void algo_set_userdata(algo al, gpointer userdata);
BYUL_API void algo_set_algo_specific(algo al, gpointer specific);

/*------------------------------------------------------------
 * 접근자 (Getter)
 *------------------------------------------------------------*/

BYUL_API cost_func algo_get_cost_func(const algo al);
BYUL_API heuristic_func algo_get_heuristic_func(const algo al);
BYUL_API gpointer algo_get_userdata(const algo al);
BYUL_API gpointer algo_get_algo_specific(const algo al);

BYUL_API const map algo_get_map(const algo al);

// ------------------------------------------------------------
// 상태 초기화
// ------------------------------------------------------------

BYUL_API void algo_reset(algo al);

// ------------------------------------------------------------
// 내부 유틸리티 - 알고리즘 멤버 기반 추상화 함수
// ------------------------------------------------------------

// frontier (탐색 대기열) 우선순위큐
BYUL_API void algo_push_frontier(algo al, const coord c, gfloat priority);
BYUL_API coord algo_pop_frontier(algo al);

// frontier (탐색 대기열) 큐
BYUL_API void     algo_append_frontier(algo al, const coord c);
BYUL_API void     algo_prepend_frontier(algo al, const coord c);
BYUL_API coord    algo_pop_frontier_head(algo al);

// frontier (탐색 대기열) 공통
BYUL_API gboolean algo_is_frontier_empty(const algo al);
// BYUL_API void algo_frontier_clear(algo al);
BYUL_API void     algo_free_frontier(algo al);

BYUL_API void algo_trim_frontier(algo al);
BYUL_API gint algo_frontier_size(const algo al);

BYUL_API gboolean algo_remove_frontier(algo al, gpointer coord_or_coord_pq);

// came_from (경로 추적)
BYUL_API void     algo_insert_came_from(algo al, 
    const coord start, const coord goal);

BYUL_API gboolean algo_contains_came_from(const algo al, const coord key);
BYUL_API coord    algo_lookup_came_from(const algo al, const coord key);

// visited (탐색 여부)
BYUL_API gboolean algo_contains_visited(const algo al, const coord c);
BYUL_API void     algo_add_visited(algo al, const coord c);

// cost_so_far (누적 비용)
BYUL_API void     algo_set_cost_so_far(algo al, const coord c, gfloat cost);

BYUL_API gboolean algo_get_cost_so_far(const algo al, 
    const coord c, gfloat* out);

// coord 리스트 조작 (역추적용)
BYUL_API GList*   prepend_coord_to_list(GList* list, const coord c);
BYUL_API void     free_coord_list(GList* list);

// BYUL_API GList*   prepend_coord_to_list(algo al, const coord c);
// BYUL_API void     free_coord_list(algo al);

BYUL_API frontier_type_t get_frontier_type(route_algotype_t algotype);
BYUL_API algo_find_func get_algo_find_func(route_algotype_t algotype);

BYUL_API void algo_reconstruct_route(const algo al, route result, 
    coord start, coord goal);

typedef struct s_coord_pq {
    coord c;
    gfloat priority;
} coord_pq_t;

typedef coord_pq_t* coord_pq;

BYUL_API coord_pq coord_pq_new();
BYUL_API coord_pq coord_pq_new_full(const coord c, gfloat prior);

BYUL_API GList* append_coord_pq_to_list(GList* gl, coord_pq cp);

BYUL_API void coord_pq_free(coord_pq cp);


BYUL_API gint coord_pq_compare(
    gconstpointer coord_pq_a, gconstpointer coord_pq_b);

#ifdef __cplusplus
}
#endif

#endif // ALGO_H
