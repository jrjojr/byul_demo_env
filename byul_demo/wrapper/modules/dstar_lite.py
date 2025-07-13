from ffi_core import ffi, C

from typing import Any

from route_finder_common import g_RouteFinderCommon
from coord import c_coord
from coord_list import c_coord_list
from coord_hash import c_coord_hash

from route import c_route
from map import c_map, MapNeighborMode

from dstar_lite_key import c_dstar_lite_key
from dstar_lite_pqueue import c_dstar_lite_pqueue

import weakref

ffi.cdef("""
typedef void (*move_func)(const coord_t* c, void* userdata);

typedef coord_list_t* (*changed_coords_func)(void* userdata);

typedef float (*cost_func)(
    const map_t* m, const coord_t* start, const coord_t* goal, void* userdata);

typedef float (*heuristic_func)(
    const coord_t* start, const coord_t* goal, void* userdata);

typedef bool (*is_coord_blocked_func)(
    const map_t* m, int x, int y, void* userdata);

typedef struct s_dstar_lite {
    // 맵
    map_t* m;

    coord_t* start;
    coord_t* goal;

    float km;
    coord_hash_t* g_table;    // coord_t** → float*
    coord_hash_t* rhs_table;  // coord_t** → float*
    
    // 탐색 프론티어 (우선순위 큐)
    dstar_lite_pqueue_t*        frontier;

    // 비용 함수
    cost_func cost_fn;
    void* cost_fn_userdata;

    is_coord_blocked_func is_blocked_fn;
    void* is_blocked_fn_userdata;

    // 휴리스틱 함수
    heuristic_func heuristic_fn;
    void* heuristic_fn_userdata;    

    // find 함수내에서 루프시에 실행될 move_fn
    move_func move_fn;
    void* move_fn_userdata;

    // find 함수내에서 루프시에 실행될 changed_coords_fn
    changed_coords_func changed_coords_fn;
    void* changed_coords_fn_userdata;

    // 초기 경로
    route_t* proto_route;

    // 실제 실시간 장애물 반영 경로
    route_t* real_route;

    // find()내에서 잠시 멈춤 실시간 이동시에 속도 대응
    int interval_msec;

    int real_loop_max_retry;

    int compute_max_retry;

    int reconstruct_max_retry;

    int proto_compute_retry_count;

    int real_compute_retry_count;

    int real_loop_retry_count;

    int reconstruct_retry_count;
    
    // 루프 강제 종료
    bool force_quit;
    
    // update_vertex_auto_range() 
    //      함수 내부에서 update_vertex_range()를 호출할때 사용
    // dstar_lite_find() 함수 내부에서 아래의 두개 함수를 호출할때 사용
    //      dstar_lite_find_target()
    //      dstar_lite_find_route_incremental()
    int max_range;

    bool debug_mode_enabled;  // ✅ 디버그 출력을 켤지 여부

    // ✅ 디버깅용: 각 좌표별 update_vertex() 호출 횟수 저장
    coord_hash_t* update_count_table;  // key: coord_t** → value: int*
} dstar_lite_t;

/**
 * @brief 기본 설정값으로 D* Lite 설정 객체를 생성합니다.
 *
 * map이 없으면 NULL
 * 
 * 이 함수는 다음과 같은 기본값으로 구성된 D* Lite 설정 구조체를 생성합니다:
 * - start : (0, 0)
 * - goal : (0, 0)
 * - km: 0.0f
 * - max_range: 0 중심좌표의 이웃만 확인한다.
 * - real_loop_max_retry: 0 반복 탐색 하지 않는다.
 * - width : 0 무한대의 맵
 * - height : 0 무한대의 맵
 * 
 *
 * 8방향, 유클리드 거리, dstar lite 비용, 디버그 모드 false
 * 생성된 설정 객체는 이후 알고리즘에 전달되어 사용됩니다.
 *
 * @return 새로 생성된 dstar_lite_t* 객체. 
 *      사용 후 dstar_lite_free()로 해제 필요.
 */
dstar_lite_t* dstar_lite_new(map_t* m);

/**
 * @brief 사용자 정의 값으로 D* Lite 설정 객체를 생성합니다.
 *
 * map이 없으면 NULL
 * 
 * @param debug_mode_enabled  디버그 모드 활성화 여부
 * @return 새로 생성된 dstar_lite_t* 객체. 
 *      사용 후 dstar_lite_free()로 해제 필요.
 */
dstar_lite_t* dstar_lite_new_full(map_t* m, coord_t* start, 
    cost_func cost_fn, heuristic_func heuristic_fn,
    bool debug_mode_enabled);

void dstar_lite_free(dstar_lite_t* dsl);

dstar_lite_t* dstar_lite_copy(dstar_lite_t* src);

coord_t* dstar_lite_get_start(const dstar_lite_t* dsl);

void  dstar_lite_set_start(dstar_lite_t* dsl, const coord_t* c);

coord_t* dstar_lite_get_goal(const dstar_lite_t* dsl);

void  dstar_lite_set_goal(dstar_lite_t* dsl, const coord_t* c);

coord_hash_t* dstar_lite_get_g_table(const dstar_lite_t* dsl);

coord_hash_t* dstar_lite_get_rhs_table(const dstar_lite_t* dsl);

dstar_lite_pqueue_t* dstar_lite_get_frontier(const dstar_lite_t* dsl);

void     dstar_lite_set_frontier(
    dstar_lite_t* dsl, dstar_lite_pqueue_t* frontier);

float dstar_lite_get_km(const dstar_lite_t* dsl);
void   dstar_lite_set_km(dstar_lite_t* dsl, float km);

int   dstar_lite_get_max_range(const dstar_lite_t* dsl);
void   dstar_lite_set_max_range(dstar_lite_t* dsl, int value);

// find_loop 함수내에서 루프시에 최대 루프 횟수
// 실시간으로 시속 4kmh로 이동할때 interval_msec에 
// 정한 횟수만큼 곱한 시간 만큼 계속 루프 돈다. 오래 돈다는 거다
// 이걸 개선 필요하다 10x10에서 100 정도는 되야 할 거같다.
int   dstar_lite_get_real_loop_max_retry(const dstar_lite_t* dsl);
void   dstar_lite_set_real_loop_max_retry(
    dstar_lite_t* dsl, int value);
    int dstar_lite_real_loop_retry_count(dstar_lite_t* dsl);

// 10x10의 맵에서 100은 되어야 잘 찾는거 같다.
int dstar_lite_get_compute_max_retry(const dstar_lite_t* dsl);
void dstar_lite_set_compute_max_retry(
    dstar_lite_t* dsl, int v);

    int dstar_lite_proto_compute_retry_count(dstar_lite_t* dsl);

    int dstar_lite_real_compute_retry_count(dstar_lite_t* dsl);

// proto route_t* 생성할때 reconstruct_route한다. 여기에 사용하는 루프
// 10x10에서 100은 오버고 10은 너무 작고 대충 40 정도면 되겠다.
int dstar_lite_get_reconstruct_max_retry(const dstar_lite_t* dsl);
void dstar_lite_set_reconstruct_max_retry(dstar_lite_t* dsl, int v);
    int dstar_lite_reconstruct_retry_count(dstar_lite_t* dsl);

bool dstar_lite_get_debug_mode_enabled(const dstar_lite_t* dsl);

void     dstar_lite_set_debug_mode_enabled(
    dstar_lite_t* dsl, bool enabled);

coord_hash_t* dstar_lite_get_update_count_table(const dstar_lite_t* dsl);

void         dstar_lite_add_update_count(
    dstar_lite_t* dsl, const coord_t* c);

void         dstar_lite_clear_update_count(dstar_lite_t* dsl);

int         dstar_lite_get_update_count(
    dstar_lite_t* dsl, const coord_t* c);

const map_t*    dstar_lite_get_map(const dstar_lite_t* dsl);
void    dstar_lite_set_map(dstar_lite_t* dsl, map_t* m);


const route_t* dstar_lite_get_proto_route(const dstar_lite_t* dsl);

const route_t* dstar_lite_get_real_route(const dstar_lite_t* dsl);

// 설정 값들 시작, 목표 ,맵등을 유지하고
// 해시테이블들과, 우선순위큐를 초기화한다.
void         dstar_lite_reset(dstar_lite_t* dsl);

int dstar_lite_get_interval_msec(dstar_lite_t* dsl);

void dstar_lite_set_interval_msec(dstar_lite_t* dsl, int interval_msec);

// 함수 포인터

float dstar_lite_cost(
    const map_t* m, const coord_t* start, const coord_t* goal, void* userdata);
cost_func    dstar_lite_get_cost_func(const dstar_lite_t* dsl);
void dstar_lite_set_cost_func(dstar_lite_t* dsl, cost_func fn);
void*    dstar_lite_get_cost_func_userdata(const dstar_lite_t* dsl);
void dstar_lite_set_cost_func_userdata(
    dstar_lite_t* dsl, void* userdata);    

bool dstar_lite_is_blocked(
    dstar_lite_t* dsl, int x, int y, void* userdata);    
is_coord_blocked_func dstar_lite_get_is_blocked_func(dstar_lite_t* dsl);
void dstar_lite_set_is_blocked_func(
    dstar_lite_t* dsl, is_coord_blocked_func fn);
void* dstar_lite_get_is_blocked_func_userdata(dstar_lite_t* dsl);
void dstar_lite_set_is_blocked_func_userdata(
    dstar_lite_t* dsl, void* userdata);

float dstar_lite_heuristic(
    const coord_t* start, const coord_t* goal, void* userdata);
heuristic_func dstar_lite_get_heuristic_func(
    const dstar_lite_t* dsl);
void         dstar_lite_set_heuristic_func(
    dstar_lite_t* dsl, heuristic_func func);
void* dstar_lite_get_heuristic_func_userdata(dstar_lite_t* dsl);
void dstar_lite_set_heuristic_func_userdata(
    dstar_lite_t* dsl, void* userdata);    

void move_to(const coord_t* c, void* userdata);
move_func dstar_lite_get_move_func(const dstar_lite_t* dsl);
void dstar_lite_set_move_func(dstar_lite_t* dsl, move_func fn);
void* dstar_lite_get_move_func_userdata(const dstar_lite_t* dsl);
void dstar_lite_set_move_func_userdata(
    dstar_lite_t* dsl, void* userdata);

// get_changed_coords_fn 콜백 예제 함수
coord_list_t* get_changed_coords(void* userdata);
changed_coords_func dstar_lite_get_changed_coords_func(
    const dstar_lite_t* dsl);
void dstar_lite_set_changed_coords_func(
    dstar_lite_t* dsl, changed_coords_func fn);
void* dstar_lite_get_changed_coords_func_userdata(
    const dstar_lite_t* dsl);
void dstar_lite_set_changed_coords_func_userdata(
    dstar_lite_t* dsl, void* userdata);

/**
 * @brief D* Lite용 우선순위 키 계산 함수
 *
 * g[s]와 rhs[s] 중 더 작은 값을 k2로 하고,
 * k1 = k2 + heuristic(start, s) + km
 *
 * @param dsl D* Lite 객체
 * @param s   대상 좌표
 * @return 계산된 dstar_lite_key_t 구조체
 */
dstar_lite_key_t* dstar_lite_calculate_key(dstar_lite_t* dsl, const coord_t* s);

void dstar_lite_init(dstar_lite_t* dsl);

/**
 * @brief 주어진 노드의 rhs 값을 재계산하고 필요시 open 리스트 갱신
 * @param al 알고리즘 컨텍스트
 * @param u 업데이트할 좌표
 */
void dstar_lite_update_vertex(dstar_lite_t* dsl, const coord_t* u);

/**
 * @brief 특정 좌표를 중심으로 주어진 범위 내 모든 좌표에 대해 update_vertex() 수행
 * 
 * @param al         알고리즘 컨텍스트
 * @param s          중심 좌표
 * @param max_range  범위 (0이면 s만 갱신)
 */
void dstar_lite_update_vertex_range(dstar_lite_t* dsl, 
    const coord_t* s, int max_range);

/**
 * @brief config에 지정된 max_range를 기준으로 update_vertex_range() 수행
 * 
 * @param al 알고리즘 컨텍스트
 * @param s 중심 좌표
 */
void dstar_lite_update_vertex_auto_range(
    dstar_lite_t* dsl, const coord_t* s);

/**
 * @brief open list에 따라 최단 경로 계산을 수행합니다.
 * 
 * @param al 알고리즘 컨텍스트
 */    
void dstar_lite_compute_shortest_route(dstar_lite_t* dsl);

/**
 * @brief 두 좌표 사이의 경로를 재구성합니다.
 *
 * g ≈ rhs 조건이 만족되었을 때, 실제 경로를 추출하여 반환합니다.
 * 조건이 만족되지 않으면 NULL을 반환합니다.
 *
 * @param dsl 알고리즘 컨텍스트
 * @return route_t* 유효한 경로 객체, 실패 시 NULL
 */
route_t* dstar_lite_reconstruct_route(dstar_lite_t* dsl);

// 1회용 경로 찾기 가장 단순한 형태 정적인 경로 찾기와 같다.
route_t* dstar_lite_find(dstar_lite_t* dsl);

// find_proto와 find_loop로 나뉜 경로 찾기를 통합했다
void dstar_lite_find_full(dstar_lite_t* dsl);

// 동적인 경로를 찾기 위해 초기 경로를 생성한다.
void dstar_lite_find_proto(dstar_lite_t* dsl);

// 동적인 경로를 찾기 위해 초기 경로로 생성된 경로를 통해 동적인 경로를 찾는다
// dstar_lite_find_proto먼저 실행되어야 한다.
// 그렇지 않으면 길찾기 실패한다.
// 왜 find_proto를 따로 실행해야 하냐고 물어보면 고급 사용자를 위해서라고 대답하겠다.
// find_proto와 find_loop사이에 어떤 작업을 하고 싶은 사람이 있을 것이다.
// 왜 콜백함수 포인터를 사용하지 않고 라고 다시 물어보면
// 그걸위해 또 리팩토링 하기 싫어서 라고 말하겠다.
// 나중에 콜백함수가 추가될수도 있다.
// 동적인 경로를 얻고 싶으면 dstar_lite_find_full()을 사용하면 된다.
void dstar_lite_find_loop(dstar_lite_t* dsl);

/**
 * @brief 주어진 경로에 포함된 모든 좌표에 대해 update_vertex 수행
 *
 * @param al 알고리즘 컨텍스트
 * @param p 갱신할 경로
 */
void dstar_lite_update_vertex_by_route(dstar_lite_t* dsl, route_t* p);

// 루프를 강제종료한다.
void dstar_lite_force_quit(dstar_lite_t* dsl);

// 강제 종료가 요청되었나?
bool dstar_lite_is_quit_forced(dstar_lite_t* dsl);

void dstar_lite_set_force_quit(dstar_lite_t* dsl, bool v);
""", override=True)

class c_dstar_lite:
    def __init__(self, m: c_map = None, start: c_coord = None,
                cost_fn=None, heuristic_fn=None, debug=False, 
                raw_ptr=None, own=False):
        
        if raw_ptr:
            self._c = raw_ptr
            self._own = own            
        elif start:
            self._c = C.dstar_lite_new_full(
                m.ptr(),
                start.ptr(),
                cost_fn if cost_fn is not None else ffi.NULL,
                heuristic_fn if heuristic_fn is not None else ffi.NULL,
                debug
            )
            self._own = True
        else:
            self._c = C.dstar_lite_new(m.ptr())
            self._own = True            

        if not self._c:
            raise MemoryError("dstar_lite allocation failed")
        
        if own:
            self._finalizer = weakref.finalize(
                self, C.dstar_lite_free, self._c)
        else:
            self._finalizer = None        

    def ptr(self):
        return self._c

    # ───── Start/Goal ─────
    def get_start(self):
        return c_coord(raw_ptr=C.dstar_lite_get_start(self._c))

    def set_start(self, coord: c_coord):
        C.dstar_lite_set_start(self._c, coord.ptr())

    def get_goal(self):
        return c_coord(raw_ptr=C.dstar_lite_get_goal(self._c))

    def set_goal(self, coord: c_coord):
        C.dstar_lite_set_goal(self._c, coord.ptr())

    # ───── 테이블 접근 ─────
    def g_table(self):
        return c_coord_hash(
            raw_ptr=C.dstar_lite_get_g_table(self._c), own=False)

    def rhs_table(self):
        return c_coord_hash(
            raw_ptr=C.dstar_lite_get_rhs_table(self._c), own=False)

    def frontier(self):
        return c_dstar_lite_pqueue(
            raw_ptr=C.dstar_lite_get_frontier(self._c), own=False)

    def set_frontier(self, frontier: c_dstar_lite_pqueue):
        C.dstar_lite_set_frontier(self._c, frontier.ptr())

    # ───── 설정값들 ─────
    def get_km(self):
        return C.dstar_lite_get_km(self._c)

    def set_km(self, v):
        C.dstar_lite_set_km(self._c, v)

    def get_max_range(self):
        return C.dstar_lite_get_max_range(self._c)

    def set_max_range(self, v):
        C.dstar_lite_set_max_range(self._c, v)

    @property
    def real_loop_max_retry(self):
        # gint   dstar_lite_get_real_loop_max_retry(const dstar_lite dsl);
        return C.dstar_lite_get_real_loop_max_retry(self.ptr())

    @real_loop_max_retry.setter
    def real_loop_max_retry(self, value:int):
        # void   dstar_lite_set_real_loop_max_retry(
        #     dstar_lite dsl, gint value);
        C.dstar_lite_set_real_loop_max_retry(self.ptr(), value)

    @property
    def compute_max_retry(self):
        # // 10x10의 맵에서 100은 되어야 잘 찾는거 같다.
        # gint dstar_lite_get_compute_max_retry(const dstar_lite dsl);
        return C.dstar_lite_get_compute_max_retry(self.ptr())

    @compute_max_retry.setter
    def compute_max_retry(self, value:int):
        # void dstar_lite_set_compute_max_retry(
        #     const dstar_lite dsl, gint v);
        C.dstar_lite_set_compute_max_retry(self.ptr(), value)

    @property
    def reconstruct_max_retry(self):
        # // proto route 생성할때 reconstruct_route한다. 여기에 사용하는 루프
        # // 10x10에서 100은 오버고 10은 너무 작고 대충 40 정도면 되겠다.
        # gint dstar_lite_get_reconstruct_max_retry(const dstar_lite dsl);
        return C.dstar_lite_get_reconstruct_max_retry(self.ptr())

    @reconstruct_max_retry.setter
    def reconstruct_max_retry(self, value:int):
        # void dstar_lite_set_reconstruct_max_retry(
        #     const dstar_lite dsl, gint v);
        C.dstar_lite_set_reconstruct_max_retry(self.ptr(), value)

    
    @property
    def proto_compute_retry_count(self):
        # gint dstar_lite_proto_compute_retry_count(dstar_lite dsl);
        return C.dstar_lite_proto_compute_retry_count(self.ptr())

    @property
    def real_compute_retry_count(self):
        # gint dstar_lite_real_compute_retry_count(dstar_lite dsl);
        return C.dstar_lite_real_compute_retry_count(self.ptr())

    @property
    def real_loop_retry_count(self):
        # gint dstar_lite_real_loop_retry_count(dstar_lite dsl);
        return C.dstar_lite_real_loop_retry_count(self.ptr())

    @property
    def reconstruct_retry_count(self):
        # gint dstar_lite_reconstruct_retry_count(dstar_lite dsl);         
        return C.dstar_lite_reconstruct_retry_count(self.ptr())


    def set_interval_msec(self, v):
        C.dstar_lite_set_interval_msec(self._c, v)

    def get_interval_msec(self):
        return C.dstar_lite_get_interval_msec(self._c)

    def set_debug_mode_enabled(self, v: bool):
        C.dstar_lite_set_debug_mode_enabled(self._c, v)

    def is_debug_mode_enabled(self):
        return bool(C.dstar_lite_get_debug_mode_enabled(self._c))

    # ───── 경로 ─────
    def get_proto_route(self):
        return c_route(
            raw_ptr=C.dstar_lite_get_proto_route(self._c), own=False)

    def get_real_route(self):
        return c_route(
            raw_ptr=C.dstar_lite_get_real_route(self._c), own=False)

    # ───── 경로 탐색 함수들 ─────
    def find(self):
        ptr = C.dstar_lite_find(self._c)
        return c_route(raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def find_proto(self):
        C.dstar_lite_find_proto(self._c)

    def find_loop(self):
        C.dstar_lite_find_loop(self._c)

    def find_full(self):
        C.dstar_lite_find_full(self._c)

    # ───── 유틸 함수 ─────
    def init(self):
        C.dstar_lite_init(self._c)

    def reset(self):
        C.dstar_lite_reset(self._c)

    def compute_shortest_route(self):
        C.dstar_lite_compute_shortest_route(self._c)

    def reconstruct_route(self):
        ptr = C.dstar_lite_reconstruct_route(self._c)
        return c_route(raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def calculate_key(self, s: c_coord):
        ptr = C.dstar_lite_calculate_key(self._c, s.ptr())
        return c_dstar_lite_key(
            raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def update_vertex(self, coord: c_coord):
        C.dstar_lite_update_vertex(self._c, coord.ptr())

    def update_vertex_range(self, center: c_coord, range_val: int):
        C.dstar_lite_update_vertex_range(self._c, center.ptr(), range_val)

    def update_vertex_auto_range(self, center: c_coord):
        C.dstar_lite_update_vertex_auto_range(self._c, center.ptr())

    def update_vertex_by_route(self, route: c_route):
        C.dstar_lite_update_vertex_by_route(self._c, route.ptr())

    # ───── 상태 통계 및 종료 ─────
    def is_quit_forced(self):
        return bool(C.dstar_lite_is_quit_forced(self._c))

    def set_force_quit(self, v: bool):
        C.dstar_lite_set_force_quit(self._c, v)

    def force_quit(self):
        C.dstar_lite_force_quit(self._c)

    def update_count_table(self):
        return c_coord_hash(
            raw_ptr=C.dstar_lite_get_update_count_table(self._c), own=False)

    def add_update_count(self, coord: c_coord):
        C.dstar_lite_add_update_count(self._c, coord.ptr())

    def get_update_count(self, coord: c_coord):
        return C.dstar_lite_get_update_count(self._c, coord.ptr())

    def clear_update_count(self):
        C.dstar_lite_clear_update_count(self._c)

    def map(self):
        return c_map(raw_ptr=C.dstar_lite_get_map(self._c), own=False)

    def set_map(self, m: c_map):
        C.dstar_lite_set_map(self._c, m.ptr())

    # ───── 메모리 관리 ─────
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

    def __repr__(self):
        return f'''c_dstar_lite(
start={self.get_start()}, goal={self.get_goal()}, km={self.get_km():.2f})'''

    # ───── 비용 함수 (cost_fn) 등록 ─────
    def set_cost_func(self, py_func: callable):
        self._py_cost_func = py_func
        # self._cost_func_userdata = userdata
        # self._ffi_cost_func_userdata = ffi.new_handle(userdata)
        # self._ffi_cost_func_userdata = (
        #     ffi.new_handle(userdata) if userdata is not None else ffi.NULL
        # )        

        @ffi.callback(
                "float(const map_t*, const coord_t*, const coord_t*, void*)")
        def _wrapped(m_ptr, s_ptr, g_ptr, udata_ptr):
            map_obj = c_map(raw_ptr=m_ptr, own=False)
            start = c_coord(raw_ptr=s_ptr)
            goal = c_coord(raw_ptr=g_ptr)
            # user = ffi.from_handle(udata_ptr)
            user = ffi.from_handle(udata_ptr) if udata_ptr else None
            return py_func(map_obj, start, goal, user)

        self._ffi_cost_func = _wrapped
        C.dstar_lite_set_cost_func(self._c, _wrapped)
        # C.dstar_lite_set_cost_func_userdata(
        #     self._c, self._ffi_cost_func_userdata)
        


    # ───── 휴리스틱 함수 (heuristic_fn) 등록 ─────
    def set_heuristic_func(self, py_func: callable):
        self._py_heuristic_func = py_func
        # self._heuristic_func_userdata = userdata
        # self._ffi_heuristic_func_userdata = ffi.new_handle(userdata)
        # self._ffi_heuristic_func_userdata = (
        #     ffi.new_handle(userdata) if userdata is not None else ffi.NULL
        # )                

        @ffi.callback("float(const coord_t*, const coord_t*, void*)")
        def _wrapped(s_ptr, g_ptr, udata_ptr):
            start = c_coord(raw_ptr=s_ptr)
            goal = c_coord(raw_ptr=g_ptr)
            user = ffi.from_handle(udata_ptr) if udata_ptr else None
            return py_func(start, goal, user)

        self._ffi_heuristic_func = _wrapped
        C.dstar_lite_set_heuristic_func(self._c, _wrapped)
        # C.dstar_lite_set_heuristic_func_userdata(
        #     self._c, self._ffi_heuristic_func_userdata)

    # ───── 장애물 여부 함수 (is_blocked_fn) 등록 ─────
    def set_is_blocked_func(self, py_func: callable):
        self._py_is_blocked_func = py_func
        # self._is_blocked_func_userdata = userdata
        # self._ffi_is_blocked_func_userdata = ffi.new_handle(userdata)
        # self._ffi_is_blocked_func_userdata = (
        #     ffi.new_handle(userdata) if userdata is not None else ffi.NULL
        # )

        @ffi.callback("bool(const map_t*, int, int, void*)")
        def _wrapped(m_ptr, x, y, udata_ptr):
            map_obj = c_map(raw_ptr=m_ptr, own=False)
            user = ffi.from_handle(udata_ptr) if udata_ptr else None
            return bool(py_func(map_obj, x, y, user))

        self._ffi_is_blocked_func = _wrapped
        C.dstar_lite_set_is_blocked_func(self._c, _wrapped)
        # C.dstar_lite_set_is_blocked_func_userdata(
        #     self._c, self._ffi_is_blocked_func_userdata)

    # ───── 이동 콜백 함수 (move_func) 등록 ─────
    def set_move_func(self, py_func: callable):
        self._py_move_func = py_func

        @ffi.callback("void(const coord_t*, void*)")
        def _wrapped(c_ptr, udata_ptr):
            coord = c_coord(raw_ptr=c_ptr)
            user = ffi.from_handle(udata_ptr) if udata_ptr else None
            py_func(coord, user)

        self._ffi_move_func = _wrapped
        C.dstar_lite_set_move_func(self._c, _wrapped)
        # C.dstar_lite_set_move_func_userdata(
        #     self._c, self._ffi_move_func_userdata)

    # ───── 동적 장애물 갱신 함수 (changed_coords_func) 등록 ─────
    def set_changed_coords_func(self, py_func: callable):
        self._py_changed_coords_func = py_func
        # self._changed_coords_func_userdata = userdata
        # self._ffi_changed_coords_func_userdata = ffi.new_handle(userdata)
        # self._ffi_changed_coords_func_userdata = (
        #     ffi.new_handle(userdata) if userdata is not None else ffi.NULL
        # )

        @ffi.callback("coord_list_t*(void*)")
        def _wrapped(udata_ptr):
            user = ffi.from_handle(udata_ptr) if udata_ptr else None
            clist = py_func(user)
            if isinstance(clist, c_coord_list):
                return clist.ptr()
            return ffi.NULL

        self._ffi_changed_coords_func = _wrapped
        C.dstar_lite_set_changed_coords_func(self._c, _wrapped)
        # C.dstar_lite_set_changed_coords_func_userdata(
        #     self._c, self._ffi_changed_coords_func_userdata)

if __name__ == '__main__':
    # m = c_map()
    # dsl = c_dstar_lite()

    # def my_cost_fn(m: c_map, s: c_coord, g: c_coord, u):
    #     return 1.0 if not m.is_blocked(g.x, g.y) else float("inf")

    # def my_heuristic_fn(s: c_coord, g: c_coord, u):
    #     return ((s.x - g.x) ** 2 + (s.y - g.y) ** 2) ** 0.5

    # def my_is_blocked_fn(m: c_map, x: int, y: int, u):
    #     return m.is_blocked(x, y)

    # def my_move_fn(c: c_coord, u):
    #     print("Moved to", c)

    # def my_changed_coords_fn(u):
    #     return c_coord_list()  # 예시: 빈 리스트 반환

    # dsl = c_dstar_lite(m)
    # dsl.set_cost_func(my_cost_fn)
    # dsl.set_heuristic_func(my_heuristic_fn)
    # dsl.set_is_blocked_func(my_is_blocked_fn)
    # dsl.set_move_func(my_move_fn)
    # dsl.set_changed_coords_func(my_changed_coords_fn)

    map = c_map(width=10, height=10, mode=MapNeighborMode.DIR_8.value)
    finder = c_dstar_lite(map)
    finder.set_debug_mode_enabled(True)

    # 1️⃣ 초기 경로
    print("\n▶ 최초 경로:")
    start = c_coord(0,0)
    goal = c_coord(9,9)
    finder.set_start(start)
    finder.set_goal(goal)

    result = finder.find()
    result.print()

    result.close()    

g_RouteFinderCommon.register_cost("dstar_lite", C.dstar_lite_cost)

g_RouteFinderCommon.register_heuristic("dstar_lite", C.dstar_lite_heuristic)
