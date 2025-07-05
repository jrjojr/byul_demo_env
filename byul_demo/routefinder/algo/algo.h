#ifndef ALGO_H
#define ALGO_H

#include "internal/algo_common.h"

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

typedef route_t* (*find_route_func)(
    map_t*, const coord_t*, const coord_t*,
    cost_func, heuristic_func, void*);

BYUL_API find_route_func get_find_route_func(route_algotype_t type);
BYUL_API route_t* find_route(
    route_algotype_t type,
    map_t* map, const coord_t* start, const coord_t* goal,
    cost_func cost, heuristic_func heuristic, void* userdata);

BYUL_API const char* get_algo_name(route_algotype_t pa);    

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
}
#endif

#endif // ALGO_H