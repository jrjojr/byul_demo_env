import weakref

from ffi_core import ffi, C

from coord import c_coord
from coord_list import c_coord_list
from coord_hash import c_coord_hash

from enum import IntEnum

class MapNeighborMode(IntEnum):
    DIR_4 = 0
    DIR_8 = 1

ffi.cdef("""
typedef enum {
    MAP_NEIGHBOR_4,
    MAP_NEIGHBOR_8
} map_neighbor_mode_t;

struct s_map {
    int width;
    int height;
    map_neighbor_mode_t mode;

    coord_hash_t* blocked_coords;
};

typedef struct s_map map_t;

// 생성자 및 소멸자
map_t* map_new();
map_t* map_new_full(int width, int height, map_neighbor_mode_t mode);
void map_free(map_t* m);

// 복사 및 비교
map_t* map_copy(const map_t* m);
uint32_t map_hash(const map_t* m);
bool map_equal(const map_t* a, const map_t* b);

// 속성 접근
int map_get_width(const map_t* m);
void map_set_width(map_t* m, int width);

int map_get_height(const map_t* m);
void map_set_height(map_t* m, int height);

map_neighbor_mode_t map_get_mode(const map_t* m);
void map_set_mode(map_t* m);

// 장애물 관련
bool map_block_coord(map_t* m, int x, int y);
bool map_unblock_coord(map_t* m, int x, int y);
bool map_is_blocked(const map_t* m, int x, int y);
bool map_is_inside(const map_t* m, int x, int y);
void map_clear(map_t* m);

// 차단 좌표 집합 반환
const coord_hash_t* map_get_blocked_coords(const map_t* m);

// 이웃 탐색
coord_list_t* map_clone_neighbors(const map_t* m, int x, int y);
coord_list_t* map_clone_neighbors_all(const map_t* m, int x, int y);
coord_list_t* map_clone_neighbors_all_range(
    map_t* m, int x, int y, int range);

coord_t* map_clone_neighbor_at_degree(const map_t* m, 
    int x, int y, double degree);
    
coord_t* map_clone_neighbor_at_goal(const map_t* m, 
    const coord_t* center, const coord_t* goal);

coord_list_t* map_clone_neighbors_at_degree_range(
    const map_t* m,
    const coord_t* center, const coord_t* goal,
    double start_deg, double end_deg,
    int range);

""")

class c_map:
    def __init__(self, raw_ptr=None, own=False,
            width=None, height=None, mode=MapNeighborMode.DIR_8):
        
        if raw_ptr:
            self._c = raw_ptr
            self._own = own
        elif width is not None and height is not None:
            self._c = C.map_new_full(width, height, mode.value)
            self._own = True
        else:
            self._c = C.map_new()
            self._own = True

        if not self._c:
            raise MemoryError("map allocation failed")


        self._finalizer = weakref.finalize(
            self, C.map_free, self._c)

    # ───── 속성 접근 ─────
    @property
    def width(self):
        return C.map_get_width(self._c)
    
    @width.setter
    def width(self, w):
        return self.set_width(w)

    @property
    def height(self):
        return C.map_get_height(self._c)
    
    @height.setter
    def height(self, h):
        self.set_height(h)

    def mode(self):
        return MapNeighborMode(C.map_get_mode(self._c))

    def set_width(self, w):
        C.map_set_width(self._c, w)

    def set_height(self, h):
        C.map_set_height(self._c, h)

    def set_mode(self, mode: MapNeighborMode):
        C.map_set_mode(self._c, mode.value)

    # ───── 장애물 관련 ─────
    def block(self, x, y):
        return bool(C.map_block_coord(self._c, x, y))

    def unblock(self, x, y):
        return bool(C.map_unblock_coord(self._c, x, y))

    def is_blocked(self, x, y):
        return bool(C.map_is_blocked(self._c, x, y))

    def is_inside(self, x, y):
        return bool(C.map_is_inside(self._c, x, y))

    def clear(self):
        C.map_clear(self._c)

    def blocked_coords(self):
        return c_coord_hash(
            raw_ptr=C.map_get_blocked_coords(self._c), own=False)

    # ───── 이웃 좌표 탐색 ─────
    def neighbors(self, x, y):
        ptr = C.map_clone_neighbors(self._c, x, y)
        return c_coord_list(raw_ptr=ptr, own=True)

    def neighbors_all(self, x, y):
        ptr = C.map_clone_neighbors_all(self._c, x, y)
        return c_coord_list(raw_ptr=ptr, own=True)

    def neighbors_range(self, x, y, range_val):
        ptr = C.map_clone_neighbors_all_range(self._c, x, y, range_val)
        return c_coord_list(raw_ptr=ptr, own=True)

    def neighbor_at_degree(self, x, y, degree):
        ptr = C.map_clone_neighbor_at_degree(self._c, x, y, degree)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def neighbor_at_goal(self, center: c_coord, goal: c_coord):
        ptr = C.map_clone_neighbor_at_goal(self._c, center.ptr(), goal.ptr())
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def neighbors_at_degree_range(self, center: c_coord, goal: c_coord,
        start_deg: float, end_deg: float, range_val: int):

        ptr = C.map_clone_neighbors_at_degree_range(
            self._c, center.ptr(), goal.ptr(), start_deg, end_deg, range_val)
        
        return c_coord_list(raw_ptr=ptr, own=True)

    # ───── 복사 및 비교 ─────
    def copy(self):
        new_ptr = C.map_copy(self._c)
        return c_map(raw_ptr=new_ptr, own=True)

    def equals(self, other):
        if not isinstance(other, c_map):
            return False
        return C.map_equal(self._c, other._c)

    def __eq__(self, other):
        return self.equals(other)

    def __hash__(self):
        return int(C.map_hash(self._c))

    def ptr(self):
        return self._c

    def __repr__(self):
        return f"c_map({self.width()}x{self.height()}, mode={self.mode().name})"

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
