from ffi_core import ffi, C
import weakref

from coord import c_coord
from coord_list import c_coord_list
from coord_hash import c_coord_hash

from enum import IntEnum

class RouteDir(IntEnum):
    UNKNOWN = 0
    RIGHT = 1
    TOP_RIGHT = 2
    TOP = 3
    TOP_LEFT = 4
    LEFT = 5
    DOWN_LEFT = 6
    DOWN = 7
    DOWN_RIGHT = 8
    COUNT = 9

ffi.cdef("""
typedef enum e_route_dir {
    ROUTE_DIR_UNKNOWN, 
    ROUTE_DIR_RIGHT,
    ROUTE_DIR_TOP_RIGHT,
    ROUTE_DIR_TOP,
    ROUTE_DIR_TOP_LEFT,
    ROUTE_DIR_LEFT,
    ROUTE_DIR_DOWN_LEFT,
    ROUTE_DIR_DOWN,
    ROUTE_DIR_DOWN_RIGHT,
    ROUTE_DIR_COUNT
} route_dir_t;

struct s_route {
    coord_list_t* coords;
    coord_list_t* visited_order;
    coord_hash_t* visited_count;
    float cost;
    bool success;
    int total_retry_count;

    float avg_vec_x;
    float avg_vec_y;
    int vec_count;
};

typedef struct s_route route_t;

/** 생성 및 해제 **/
route_t* route_new(void);
route_t* route_new_full(float cost);
void  route_free(route_t* p);

/** 복사 및 비교 **/
route_t* route_copy(const route_t* p);
uintptr_t route_hash(const route_t* a);
int route_equal(const route_t* a, const route_t* b);

/** 기본 정보 **/
void  route_set_cost(route_t* p, float cost);
float route_get_cost(const route_t* p);
void  route_set_success(route_t* p, int success);
int   route_get_success(const route_t* p);

/** 좌표 리스트 접근 **/
const coord_list_t* route_get_coords(const route_t* p);

/** 방문 로그 **/
const coord_list_t* route_get_visited_order(const route_t* p);
const coord_hash_t*  route_get_visited_count(const route_t* p);

int route_get_total_retry_count(const route_t* p);

void route_set_total_retry_count(route_t* p, int retry_count);

/** 좌표 조작 **/
int  route_add_coord(route_t* p, const coord_t* c);
void route_clear_coords(route_t* p);
const coord_t* route_get_last(const route_t* p);
const coord_t* route_get_coord_at(const route_t* p, int index);
int   route_length(const route_t* p);

/** 방문 조작 **/
int  route_add_visited(route_t* p, const coord_t* c);
void route_clear_visited(route_t* p);

/** 병합 및 편집 **/
void route_append(route_t dest, const route_t* src);
void route_append_nodup(route_t dest, const route_t* src);

void route_insert(route_t* p, int index, const coord_t* c);
void route_remove_at(route_t* p, int index);
void route_remove_value(route_t* p, const coord_t* c);
int  route_contains(const route_t* p, const coord_t* c);
int  route_find(const route_t* p, const coord_t* c);
void route_slice(route_t* p, int start, int end);

/** 출력 및 디버깅 **/
void route_print(const route_t* p);

/** 방향 계산 **/
coord_t* route_make_direction(route_t* p, int index);
route_dir_t route_get_direction_by_coord(const coord_t* dxdy);
route_dir_t route_get_direction_by_index(route_t* p, int index);
route_dir_t route_calc_average_facing(route_t* p, int history);
float route_calc_average_dir(route_t* p, int history);

coord_t* direction_to_coord(route_dir_t route_dir);

/** 방향 변화 판단 **/
int route_has_changed(
    route_t* p, const coord_t* from,
    const coord_t* to, float angle_threshold_deg);

int route_has_changed_with_angle(
    route_t* p, const coord_t* from,
    const coord_t* to, float angle_threshold_deg,
    float* out_angle_deg);

int route_has_changed_by_index(
    route_t* p, int index_from,
    int index_to, float angle_threshold_deg);

int route_has_changed_with_angle_by_index(
    route_t* p, int index_from, int index_to,
    float angle_threshold_deg, float* out_angle_deg);

/** 평균 벡터 누적 **/
void route_update_average_vector(
    route_t* p, const coord_t* from, const coord_t* to);

void route_update_average_vector_by_index(
    route_t* p, int index_from, int index_to);

route_dir_t calc_direction(
    const coord_t* start, const coord_t* goal);

/// @brief goal → start 방향으로 came_from을 따라 경로를 복원하여 route에 채운다.
/// @param route 결과 경로 구조체 (output)
/// @param came_from coord_hash_t* (coord* → coord*)
/// @param start 시작 좌표
/// @param goal 도착 좌표
/// @return 성공 여부 (true: 경로 복원 성공, false: 실패)
bool route_reconstruct_path(
    route_t* route, const coord_hash_t* came_from,
    const coord_t* start, const coord_t* goal);

""")

class c_route:
    def __init__(self, raw_ptr=None, cost=None, own=False):
        if raw_ptr:
            self._c = raw_ptr
            self._own = own
        elif cost is not None:
            self._c = C.route_new_full(cost)
            self._own = True
        else:
            self._c = C.route_new()
            self._own = True

        if not self._c:
            raise MemoryError("route allocation failed")

        self._finalizer = weakref.finalize(self, C.route_free, self._c)

    # ───── 기본 정보 ─────
    def cost(self):
        return C.route_get_cost(self._c)

    def set_cost(self, cost: float):
        C.route_set_cost(self._c, cost)

    def is_success(self):
        return bool(C.route_get_success(self._c))

    def set_success(self, success: bool):
        C.route_set_success(self._c, int(success))

    def retry_count(self):
        return C.route_get_total_retry_count(self._c)

    def set_retry_count(self, count: int):
        C.route_set_total_retry_count(self._c, count)

    # ───── 경로 좌표 ─────
    def coords(self):
        return c_coord_list(raw_ptr=C.route_get_coords(self._c), own=False)

    def add_coord(self, coord: c_coord):
        return C.route_add_coord(self._c, coord.ptr())

    def clear_coords(self):
        C.route_clear_coords(self._c)

    def last(self):
        ptr = C.route_get_last(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def coord_at(self, index):
        ptr = C.route_get_coord_at(self._c, index)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def length(self):
        return C.route_length(self._c)

    # ───── 방문 로그 ─────
    def visited_order(self):
        return c_coord_list(raw_ptr=C.route_get_visited_order(self._c), own=False)

    def visited_count(self):
        return c_coord_hash(raw_ptr=C.route_get_visited_count(self._c), own=False)

    def add_visited(self, coord: c_coord):
        return C.route_add_visited(self._c, coord.ptr())

    def clear_visited(self):
        C.route_clear_visited(self._c)

    # ───── 경로 병합 및 편집 ─────
    def append(self, other: 'c_route', nodup=False):
        if nodup:
            C.route_append_nodup(self._c, other._c)
        else:
            C.route_append(self._c, other._c)

    def insert(self, index, coord: c_coord):
        C.route_insert(self._c, index, coord.ptr())

    def remove_at(self, index):
        C.route_remove_at(self._c, index)

    def remove_value(self, coord: c_coord):
        C.route_remove_value(self._c, coord.ptr())

    def contains(self, coord: c_coord):
        return bool(C.route_contains(self._c, coord.ptr()))

    def find(self, coord: c_coord):
        return C.route_find(self._c, coord.ptr())

    def slice(self, start, end):
        C.route_slice(self._c, start, end)

    # ───── 방향 처리 ─────
    def look_at(self, index):
        ptr = C.route_make_direction(self._c, index)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def get_direction_by_coord(self, dxdy: c_coord):
        return RouteDir(C.route_get_direction_by_coord(dxdy.ptr()))

    def get_direction_by_index(self, index):
        return RouteDir(C.route_get_direction_by_index(self._c, index))

    def calc_average_facing(self, history):
        return RouteDir(C.route_calc_average_facing(self._c, history))

    def calc_average_dir(self, history):
        return C.route_calc_average_dir(self._c, history)

    # ───── 방향 변화 판단 ─────
    def has_changed(self, from_coord, to_coord, angle_threshold):
        return bool(C.route_has_changed(self._c, from_coord.ptr(), to_coord.ptr(), angle_threshold))

    def has_changed_with_angle(self, from_coord, to_coord, angle_threshold):
        out = ffi.new("float*")
        changed = C.route_has_changed_with_angle(self._c, from_coord.ptr(), to_coord.ptr(), angle_threshold, out)
        return bool(changed), out[0]

    def has_changed_by_index(self, index_from, index_to, angle_threshold):
        return bool(C.route_has_changed_by_index(self._c, index_from, index_to, angle_threshold))

    def has_changed_with_angle_by_index(self, index_from, index_to, angle_threshold):
        out = ffi.new("float*")
        changed = C.route_has_changed_with_angle_by_index(self._c, index_from, index_to, angle_threshold, out)
        return bool(changed), out[0]

    def update_average_vector(self, from_coord, to_coord):
        C.route_update_average_vector(self._c, from_coord.ptr(), to_coord.ptr())

    def update_average_vector_by_index(self, index_from, index_to):
        C.route_update_average_vector_by_index(self._c, index_from, index_to)

    def reconstruct_path(self, came_from: c_coord_hash, start: c_coord, goal: c_coord):
        return bool(C.route_reconstruct_path(self._c, came_from.ptr(), start.ptr(), goal.ptr()))

    def print(self):
        C.route_print(self._c)

    def ptr(self):
        return self._c

    def __repr__(self):
        return f"c_route(len={self.length()}, cost={self.cost():.2f}, success={self.is_success()})"

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

def direction_to_coord(direction: RouteDir) -> c_coord:
    ptr = C.direction_to_coord(direction)
    return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

def calc_direction(start: c_coord, goal: c_coord) -> RouteDir:
    return RouteDir(C.calc_direction(start.ptr(), goal.ptr()))
