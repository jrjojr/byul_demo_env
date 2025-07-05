#include "internal/algo.h"
#include "internal/cost_coord_pq.h"
#include "internal/coord_list.h"
#include "coord.hpp"

#include <cmath>
#include <vector>
#include <limits>

// 비용 함수

float default_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata) {
    return 1.0f;
}

float zero_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata) {
    return 0.0f;
}

float diagonal_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata) {
    if (!start || !goal) return std::numeric_limits<float>::max();
    int dx = std::abs(start->x - goal->x);
    int dy = std::abs(start->y - goal->y);
    return (dx != 0 && dy != 0) ? DIAGONAL_COST : 1.0f;
}

// 휴리스틱 함수

float euclidean_heuristic(
    const coord_t* start, const coord_t* goal, void* userdata) {

    if (!start || !goal) return std::numeric_limits<float>::max();
    int dx = start->x - goal->x;
    int dy = start->y - goal->y;
    return std::sqrt(static_cast<float>(dx * dx + dy * dy));
}

float manhattan_heuristic(
    const coord_t* start, const coord_t* goal, void* userdata) {

    if (!start || !goal) return std::numeric_limits<float>::max();
    return static_cast<float>(
        std::abs(start->x - goal->x) + std::abs(start->y - goal->y));
}

float chebyshev_heuristic(
    const coord_t* start, const coord_t* goal, void* userdata) {

    if (!start || !goal) return std::numeric_limits<float>::max();
    int dx = std::abs(start->x - goal->x);
    int dy = std::abs(start->y - goal->y);
    return static_cast<float>(std::max(dx, dy));
}

float octile_heuristic(
    const coord_t* start, const coord_t* goal, void* userdata) {

    if (!start || !goal) return std::numeric_limits<float>::max();
    int dx = std::abs(start->x - goal->x);
    int dy = std::abs(start->y - goal->y);
    float F = std::sqrt(2.0f) - 1.0f;
    return static_cast<float>(std::max(dx, dy) + F * std::min(dx, dy));
}

float zero_heuristic(
    const coord_t* start, const coord_t* goal, void* userdata) {

    return 0.0f;
}

float default_heuristic(
    const coord_t* start, const coord_t* goal, void* userdata) {
        
    return euclidean_heuristic(start, goal, userdata);
}

// 알고리즘 이름 반환

const char* get_algo_name(route_algotype_t pa) {
    switch (pa) {
        case PATH_ALGO_BFS: return "bfs";
        case PATH_ALGO_DFS: return "dfs";
        case PATH_ALGO_DIJKSTRA: return "dijkstra";
        case PATH_ALGO_ASTAR: return "astar";
        case PATH_ALGO_WEIGHTED_ASTAR: return "weighted_astar";
        case PATH_ALGO_GREEDY_BEST_FIRST: return "greedy_best_first";
        case PATH_ALGO_IDA_STAR: return "ida_star";
        case PATH_ALGO_RTA_STAR: return "rta_star";
        case PATH_ALGO_SMA_STAR: return "sma_star";
        case PATH_ALGO_FAST_MARCHING: return "fast_marching";
        case PATH_ALGO_FRINGE_SEARCH: return "fringe_search";
        case PATH_ALGO_DSTAR_LITE: return "dstar_lite";
        case PATH_ALGO_DSTAR: return "dynamic_astar";
        case PATH_ALGO_LPA_STAR: return "lpa_star";
        case PATH_ALGO_HPA_STAR: return "hpa_star";
        case PATH_ALGO_ANY_ANGLE_ASTAR: return "any_angle_astar";
        case PATH_ALGO_ALT: return "alt";
        case PATH_ALGO_THETA_STAR: return "theta_star";
        case PATH_ALGO_LAZY_THETA_STAR: return "lazy_theta_star";
        case PATH_ALGO_JUMP_POINT_SEARCH: return "jump_point_search";
        case PATH_ALGO_JPS_PLUS: return "jps_plus";
        case PATH_ALGO_BIDIRECTIONAL_ASTAR: return "bidirectional_astar";
        default: return "unknown";
    }
}
