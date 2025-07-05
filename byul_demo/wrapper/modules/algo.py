from ffi_core import ffi, C

from enum import IntEnum

class RouteAlgotype(IntEnum):
    ROUTE_ALGO_UNKNOWN = 0

    # // 1950s~1960s
    ROUTE_ALGO_BELLMAN_FORD #            // 1958
    ROUTE_ALGO_DFS #                     // 1959
    ROUTE_ALGO_BFS #                     // 1959
    ROUTE_ALGO_DIJKSTRA #                // 1959
    ROUTE_ALGO_FLOYD_WARSHALL #          // 1959~
    ROUTE_ALGO_ASTAR #                   // 1968

    # // 1970s
    ROUTE_ALGO_BIDIRECTIONAL_DIJKSTRA # ,  // 1971
    ROUTE_ALGO_BIDIRECTIONAL_ASTAR #,     // 1971
    ROUTE_ALGO_WEIGHTED_ASTAR #,          // 1977~
    ROUTE_ALGO_JOHNSON #,                 // 1977
    ROUTE_ALGO_K_SHORTEST_PATH #,         // 1977~
    ROUTE_ALGO_DIAL #,                    // 1969

    # // 1980s
    ROUTE_ALGO_ITERATIVE_DEEPENING #,     // 1980
    ROUTE_ALGO_GREEDY_BEST_FIRST #,       // 1985
    ROUTE_ALGO_IDA_STAR #,                // 1985

    # // 1990s
    ROUTE_ALGO_RTA_STAR #,                // 1990
    ROUTE_ALGO_SMA_STAR #,                // 1991
    ROUTE_ALGO_DSTAR #,                   // 1994
    ROUTE_ALGO_FAST_MARCHING #,           // 1996
    ROUTE_ALGO_ANT_COLONY #,              // 1996
    ROUTE_ALGO_FRINGE_SEARCH #,           // 1997

    # // 2000s
    ROUTE_ALGO_FOCAL_SEARCH #,            // 2001
    ROUTE_ALGO_DSTAR_LITE #,              // 2002
    ROUTE_ALGO_LPA_STAR #,                // 2004
    ROUTE_ALGO_HPA_STAR #,                // 2004
    ROUTE_ALGO_ALT #,                     // 2005
    ROUTE_ALGO_ANY_ANGLE_ASTAR #,         // 2005~
    ROUTE_ALGO_HCA_STAR #,                // 2005
    ROUTE_ALGO_RTAA_STAR #,               // 2006
    ROUTE_ALGO_THETA_STAR #,              // 2007
    ROUTE_ALGO_CONTRACTION_HIERARCHIES # ,// 2008

    # // 2010s
    ROUTE_ALGO_LAZY_THETA_STAR #,         // 2010
    ROUTE_ALGO_JUMP_POINT_SEARCH #,       // 2011
    ROUTE_ALGO_SIPP #,                    // 2011
    ROUTE_ALGO_JPS_PLUS #,                // 2012
    ROUTE_ALGO_EPEA_STAR #,               // 2012
    ROUTE_ALGO_MHA_STAR #              // 2012
    ROUTE_ALGO_ANYA #,                    // 2013

    # // 특수 목적 / 확장형
    ROUTE_ALGO_DAG_SP #,                  // 1960s (DAG 최단경로 O(V+E))
    ROUTE_ALGO_MULTI_SOURCE_BFS #,        // 2000s (복수 시작점 BFS)
    ROUTE_ALGO_MCTS #                     // 2006

ffi.cdef("""
#define DIAGONAL_COST 1.4142135f  // √2 근사값

/*------------------------------------------------------------
 * 함수 포인터
 *------------------------------------------------------------*/

typedef float (*cost_func)(
    const map_t* m, const coord_t* start, const coord_t* goal, void* userdata);

typedef float (*heuristic_func)(
    const coord_t* start, const coord_t* goal, void* userdata);

/** 비용 계산 함수들 */
float default_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata);

float zero_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata);

float diagonal_cost(const map_t* m, 
    const coord_t* start, const coord_t* goal, void* userdata);

/** 유클리드 거리 */
float euclidean_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 맨해튼 거리 */
float manhattan_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 체비셰프 거리 */
float chebyshev_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 옥타일 거리 (8방향 이동 가중치) */
float octile_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/** 제로 거리 (테스트/그리디) */
float zero_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

float default_heuristic(const coord_t* start, const coord_t* goal, 
    void* userdata);

/*------------------------------------------------------------
 * 알고리즘 종류 정의
 *------------------------------------------------------------*/
typedef enum {
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

typedef enum {
    FRONTIER_QUEUE,
    FRONTIER_PRIORQ
} frontier_type_t;

const char* get_algo_name(route_algotype_t pa);

""")

class c_algo:
    def __init__(self, algotype: RouteAlgotype):
        self.algotype = algotype

    def name(self) -> str:
        name_ptr = C.get_algo_name(self.algotype)
        return ffi.string(name_ptr).decode(
            "utf-8") if name_ptr != ffi.NULL else "UNKNOWN"

    def __str__(self):
        return f"{self.name()} ({self.algotype.name})"

    def __repr__(self):
        return f"c_algo(type={self.algotype.name})"

    @staticmethod
    def get_cost_func(name: str):
        """문자열로 비용 함수 반환"""
        table = {
            "default": C.default_cost,
            "zero": C.zero_cost,
            "diagonal": C.diagonal_cost,
        }
        return table.get(name)

    @staticmethod
    def get_heuristic_func(name: str):
        """문자열로 휴리스틱 함수 반환"""
        table = {
            "euclidean": C.euclidean_heuristic,
            "manhattan": C.manhattan_heuristic,
            "chebyshev": C.chebyshev_heuristic,
            "octile": C.octile_heuristic,
            "zero": C.zero_heuristic,
            "default": C.default_heuristic,
        }
        return table.get(name)

    @staticmethod
    def list_algos():
        """enum 전체 목록 반환"""
        return [e for e in RouteAlgotype]

    @staticmethod
    def find_by_name(name: str):
        """이름으로 algotype 찾기"""
        for a in RouteAlgotype:
            algo = c_algo(a)
            if algo.name().lower() == name.lower():
                return a
        return RouteAlgotype.ROUTE_ALGO_UNKNOWN
