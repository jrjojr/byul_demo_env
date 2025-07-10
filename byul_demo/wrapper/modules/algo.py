from ffi_core import ffi, C

from coord import c_coord
from coord_list import c_coord_list
from coord_hash import c_coord_hash

from route import c_route
from map import c_map
from algo_common import g_AlgoCommon

import weakref

from enum import IntEnum

class RouteAlgotype(IntEnum):
    UNKNOWN = 0

    # // 1950s~1960s
    BELLMAN_FORD = 1 #            // 1958
    DFS = 2 #                     // 1959
    BFS = 3 #                     // 1959
    DIJKSTRA = 4 #                // 1959
    FLOYD_WARSHALL = 5 #          // 1959~
    ASTAR = 6 #                   // 1968

    # // 1970s
    BIDIRECTIONAL_DIJKSTRA = 7 # ,  // 1971
    BIDIRECTIONAL_ASTAR =8 #,     // 1971
    WEIGHTED_ASTAR = 9 #,          // 1977~
    JOHNSON = 10 #,                 // 1977
    K_SHORTEST_PATH = 11 #,         // 1977~
    DIAL = 12 #,                    // 1969

    # // 1980s
    ITERATIVE_DEEPENING = 13 #,     // 1980
    GREEDY_BEST_FIRST = 14 #,       // 1985
    IDA_STAR = 15 #,                // 1985

    # // 1990s
    RTA_STAR = 16 #,                // 1990
    SMA_STAR = 17 #,                // 1991
    DSTAR = 18 #,                   // 1994
    FAST_MARCHING = 19 #,           // 1996
    ANT_COLONY = 20 #,              // 1996
    FRINGE_SEARCH = 21 #,           // 1997

    # // 2000s
    FOCAL_SEARCH = 22 #,            // 2001
    DSTAR_LITE = 23 #,              // 2002
    LPA_STAR = 24 #,                // 2004
    HPA_STAR = 25 #,                // 2004
    ALT = 26 #,                     // 2005
    ANY_ANGLE_ASTAR = 27 #,         // 2005~
    HCA_STAR = 28 #,                // 2005
    RTAA_STAR = 29 #,               // 2006
    THETA_STAR = 30 #,              // 2007
    CONTRACTION_HIERARCHIES = 31 # ,// 2008

    # // 2010s
    LAZY_THETA_STAR = 32 #,         // 2010
    JUMP_POINT_SEARCH = 33 #,       // 2011
    SIPP = 34 #,                    // 2011
    JPS_PLUS = 35 #,                // 2012
    EPEA_STAR = 36 #,               // 2012
    MHA_STAR = 37 #              // 2012
    ANYA = 38 #,                    // 2013

    # // 특수 목적 / 확장형
    DAG_SP = 39 #,                  // 1960s (DAG 최단경로 O(V+E))
    MULTI_SOURCE_BFS =40 #,        // 2000s (복수 시작점 BFS)
    MCTS =41 #                     // 2006

ffi.cdef("""
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


const char* get_algo_name(route_algotype_t pa);

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
algo_t* algo_new(void);

algo_t* algo_new_full(map_t* map, route_algotype_t type, 
    coord_t* start, coord_t* goal,
    cost_func cost_fn, heuristic_func heuristic_fn,
    int max_retry, bool visited_logging, void* userdata);

void algo_free(algo_t* a);

algo_t* algo_copy(const algo_t* src);

/**
 * @brief 설정값 세터/게터
 */
void algo_set_map(algo_t* a, map_t* map);
void algo_set_start(algo_t* a, coord_t* start);
void algo_set_goal(algo_t* a, coord_t* goal);

map_t* algo_get_map(const algo_t* a);
coord_t* algo_get_start(const algo_t* a);
coord_t* algo_get_goal(const algo_t* a);

void algo_set_userdata(algo_t* a, void* userdata);
void* algo_get_userdata(const algo_t* a);
         
void algo_set_type(algo_t* a, route_algotype_t type);
route_algotype_t algo_get_type(const algo_t* a);
         
void algo_set_visited_logging(algo_t* a, bool is_logging);
bool algo_is_visited_logging(algo_t* a);         
         
void algo_set_cost_func(algo_t* a, cost_func cost_fn);
cost_func algo_get_cost_func(algo_t* a);

void algo_set_heuristic_func(algo_t* a, heuristic_func heuristic_fn);
heuristic_func algo_get_heuristic_func(algo_t* a);

void algo_set_max_retry(algo_t* a, int max_retry);
int algo_get_max_retry(algo_t* a);         
         
/**
 * @brief 설정값 기본화 및 검증
 */
void algo_clear(algo_t* a);

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
void algo_set_defaults(algo_t* a);

bool algo_is_valid(const algo_t* a);
void algo_print(const algo_t* a);

/**
 * @brief 정적 길찾기 실행 (알고리즘 유형 분기 포함)
 */
route_t* algo_find_with_type(const algo_t* a, route_algotype_t type);

route_t* algo_find(const algo_t* a);         

/**
 * @brief 알고리즘별 직접 실행 함수 (정적 길찾기 전용)
 */
route_t* algo_find_astar(const algo_t* a);
route_t* algo_find_bfs(const algo_t* a);
route_t* algo_find_dfs(const algo_t* a);
route_t* algo_find_dijkstra(const algo_t* a);

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
route_t* algo_find_fringe_search(const algo_t* a);

route_t* algo_find_greedy_best_first(const algo_t* a);
route_t* algo_find_ida_star(const algo_t* a);

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
route_t* algo_find_rta_star(const algo_t* a);

route_t* algo_find_sma_star(const algo_t* a);

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
route_t* algo_find_weighted_astar(const algo_t* a);

route_t* algo_find_fast_marching(const algo_t* a);

""")

class c_algo:
    def __init__(self, 
                 map: c_map,
                 type: RouteAlgotype = RouteAlgotype.ASTAR,
                 start: c_coord = None,
                 goal: c_coord = None,
                 cost_fn = None,
                 heuristic_fn = None,
                 max_retry: int = 10000,
                 visited_logging: bool = False,
                 userdata = None,
                 raw_ptr = None, own=False):
        
        if raw_ptr:
            self._c = raw_ptr
            self._own = own
        else:
            if not start or not goal:
                start = start or c_coord(0, 0)
                goal = goal or c_coord(0, 0)
            if not cost_fn:
                cost_fn = C.default_cost
            if not heuristic_fn:
                heuristic_fn = C.euclidean_heuristic

            self._c = C.algo_new_full(
                map.ptr(),
                type.value,
                start._c,
                goal._c,
                cost_fn,
                heuristic_fn,
                max_retry,
                visited_logging,
                ffi.NULL if userdata is None else ffi.new_handle(userdata)
            )
            self._own = True
        
        if not self._c:
            raise MemoryError("algo allocation failed")

        if own:
            self._finalizer = weakref.finalize(
                self, C.algo_free, self._c)
        else:
            self._finalizer = None        
        
        self.type = type

    def ptr(self):
        return self._c

    def copy(self):
        ptr = C.algo_copy(self._c)
        return c_algo(raw_ptr=ptr, own=True)

    def find_with_type(self, type:RouteAlgotype):
        ptr = C.algo_find_with_type(self._c, type)
        return c_route(raw_ptr=ptr, own=True) if ptr != ffi.NULL else None
            
    def find(self):
        ptr = C.algo_find(self._c)
        return c_route(raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def set_type(self, type:RouteAlgotype):
        # void algo_set_type(algo_t* a, route_algotype_t type);
        C.algo_set_type(self.ptr(), type.value)

    def get_type(self):
        # route_algotype_t algo_get_type(algo_t* a);        
        return RouteAlgotype(C.algo_get_type(self.ptr()))
    
    def is_visited_logging(self):
        # bool algo_is_visited_logging(algo_t* a);         
        return C.algo_is_visited_logging(self.ptr())

    def set_visited_logging(self, is_logging:bool):
        # void algo_set_visited_logging(algo_t* a, bool is_logging);
        C.algo_set_visited_logging(self.ptr(), is_logging)

    def set_max_retry(self, max_retry:int):
        # void algo_set_max_retry(algo_t* a, int max_retry);
        C.algo_set_max_retry(self.ptr(), max_retry)

    def get_max_retry(self):
        # int algo_get_max_retry(algo_t* a);         
        return C.algo_get_max_retry(self.ptr())

    def set_map(self, map: c_map):
        C.algo_set_map(self._c, map.ptr())

    def set_start(self, coord: c_coord):
        C.algo_set_start(self._c, coord._c)

    def set_goal(self, coord: c_coord):
        C.algo_set_goal(self._c, coord._c)

    def set_userdata(self, obj):
        handle = ffi.new_handle(obj)
        C.algo_set_userdata(self._c, handle)

    def get_start(self):
        return c_coord(raw_ptr=ffi.new("coord_t*", C.algo_get_start(self._c)))

    def get_goal(self):
        return c_coord(raw_ptr=ffi.new("coord_t*", C.algo_get_goal(self._c)))

    def get_map(self):
        return c_map(raw_ptr=C.algo_get_map(self._c), own=False)

    def clear(self):
        C.algo_clear(self._c)

    def is_valid(self):
        return bool(C.algo_is_valid(self._c))

    def print(self):
        C.algo_print(self._c)

    def __str__(self):
        return self.name()

    def __repr__(self):
        return f"c_algo(type={self.algotype.name})"

    def name(self):
        name_ptr = C.get_algo_name(self.algotype)
        return ffi.string(name_ptr).decode(
            "utf-8") if name_ptr != ffi.NULL else "UNKNOWN"

    def __del__(self):
        if self._own and self._finalizer and self._finalizer.alive:
            self._finalizer()

    def close(self):
        if self._own and self._finalizer and self._finalizer.alive:
            self._finalizer()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.close()

    @staticmethod
    def list_algos():
        return list(RouteAlgotype)

    @staticmethod
    def find_by_name(name: str):
        for a in RouteAlgotype:
            algo = c_algo(a)
            if algo.name().lower() == name.lower():
                return a
        return RouteAlgotype.UNKNOWN
    
    def set_cost_func(self, func_name:str):
        # void algo_set_cost_func(algo_t* a, cost_func cost_fn);
        # cost_func algo_get_cost_func(algo_t* a);
        cost_fn = g_AlgoCommon.get_cost(func_name)
        C.algo_set_cost_func(self.ptr(), cost_fn)
        pass

    def set_heuristic_func(self, func_name:str):
        # void algo_set_heuristic_func(algo_t* a, heuristic_func heuristic_fn);
        # heuristic_func algo_get_heuristic_func(algo_t* a);
        heuristic_fn = g_AlgoCommon.get_heuristic(func_name)
        C.algo_set_heuristic_func(self.ptr(), heuristic_fn)
        pass
