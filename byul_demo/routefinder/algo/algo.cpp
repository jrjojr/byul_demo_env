#include "internal/algo.h"
#include "internal/cost_coord_pq.h"
#include "internal/coord_list.h"
#include "coord.hpp"

#include <cmath>
#include <vector>
#include <limits>

const char* get_algo_name(route_algotype_t pa) {
    switch (pa) {
        case ROUTE_ALGO_BFS: return "bfs";
        case ROUTE_ALGO_DFS: return "dfs";
        case ROUTE_ALGO_DIJKSTRA: return "dijkstra";
        case ROUTE_ALGO_ASTAR: return "astar";
        case ROUTE_ALGO_WEIGHTED_ASTAR: return "weighted_astar";
        case ROUTE_ALGO_GREEDY_BEST_FIRST: return "greedy_best_first";
        case ROUTE_ALGO_IDA_STAR: return "ida_star";
        case ROUTE_ALGO_RTA_STAR: return "rta_star";
        case ROUTE_ALGO_SMA_STAR: return "sma_star";
        case ROUTE_ALGO_FAST_MARCHING: return "fast_marching";
        case ROUTE_ALGO_FRINGE_SEARCH: return "fringe_search";
        case ROUTE_ALGO_DSTAR_LITE: return "dstar_lite";
        case ROUTE_ALGO_DSTAR: return "dynamic_astar";
        case ROUTE_ALGO_LPA_STAR: return "lpa_star";
        case ROUTE_ALGO_HPA_STAR: return "hpa_star";
        case ROUTE_ALGO_ANY_ANGLE_ASTAR: return "any_angle_astar";
        case ROUTE_ALGO_ALT: return "alt";
        case ROUTE_ALGO_THETA_STAR: return "theta_star";
        case ROUTE_ALGO_LAZY_THETA_STAR: return "lazy_theta_star";
        case ROUTE_ALGO_JUMP_POINT_SEARCH: return "jump_point_search";
        case ROUTE_ALGO_JPS_PLUS: return "jps_plus";
        case ROUTE_ALGO_BIDIRECTIONAL_ASTAR: return "bidirectional_astar";
        default: return "unknown";
    }
}
