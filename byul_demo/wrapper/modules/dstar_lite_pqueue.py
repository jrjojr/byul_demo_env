from ffi_core import ffi, C

from coord import c_coord
from dstar_lite_key import c_dstar_lite_key

import weakref

ffi.cdef("""
typedef struct s_dstar_lite_pqueue dstar_lite_pqueue_t;

// ------------------------ 생성/소멸/복사 ------------------------

dstar_lite_pqueue_t* dstar_lite_pqueue_new(void);

void dstar_lite_pqueue_free(dstar_lite_pqueue_t* q);

dstar_lite_pqueue_t* dstar_lite_pqueue_copy(const dstar_lite_pqueue_t* src);

// ------------------------ 삽입/조회/삭제 ------------------------

void dstar_lite_pqueue_push(
    dstar_lite_pqueue_t* q,
    const dstar_lite_key_t* key,
    const coord_t* c);

coord_t* dstar_lite_pqueue_peek(dstar_lite_pqueue_t* q);

coord_t* dstar_lite_pqueue_pop(dstar_lite_pqueue_t* q);

bool dstar_lite_pqueue_is_empty(dstar_lite_pqueue_t* q);

// ------------------------ 삭제/조회 함수 ------------------------

/// @brief 해당 coord_t* 를 가진 요소 제거
bool dstar_lite_pqueue_remove(dstar_lite_pqueue_t* q, const coord_t* u);

/// @brief key + coord 쌍이 정확히 일치하는 항목 제거
bool dstar_lite_pqueue_remove_full(
    dstar_lite_pqueue_t* q,
    const dstar_lite_key_t* key,
    const coord_t* c);

/// @brief coord에 해당하는 key 복사본 반환 (없으면 NULL)
dstar_lite_key_t* dstar_lite_pqueue_get_key_by_coord(
    dstar_lite_pqueue_t* q, const coord_t* c);

/// @brief top 우선순위의 key 복사본 반환
dstar_lite_key_t* dstar_lite_pqueue_top_key(dstar_lite_pqueue_t* q);

/// @brief 해당 coord가 큐에 존재하는지 확인
bool dstar_lite_pqueue_contains(
    dstar_lite_pqueue_t* q, const coord_t* u);

""")

class c_dstar_lite_pqueue:
    def __init__(self, raw_ptr=None, own=False):
        if raw_ptr:
            self._c = raw_ptr
            self._own = own
        else:
            self._c = C.dstar_lite_pqueue_new()
            if not self._c:
                raise MemoryError("dstar_lite_pqueue allocation failed")
            self._own = True
        self._finalizer = weakref.finalize(
            self, C.dstar_lite_pqueue_free, self._c)

    def push(self, key: c_dstar_lite_key, coord: c_coord):
        C.dstar_lite_pqueue_push(self._c, key.ptr(), coord.ptr())

    def peek(self):
        ptr = C.dstar_lite_pqueue_peek(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def pop(self):
        ptr = C.dstar_lite_pqueue_pop(self._c)
        x = C.coord_get_x(ptr)
        y = C.coord_get_y(ptr)
        return c_coord(x, y) if ptr != ffi.NULL else None

    def top_key(self):
        ptr = C.dstar_lite_pqueue_top_key(self._c)
        return c_dstar_lite_key(
            raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def find_key_by_coord(self, coord: c_coord):
        ptr = C.dstar_lite_pqueue_get_key_by_coord(self._c, coord.ptr())
        return c_dstar_lite_key(
            raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def remove(self, coord: c_coord):
        return bool(C.dstar_lite_pqueue_remove(self._c, coord.ptr()))

    def remove_full(self, key: c_dstar_lite_key, coord: c_coord):
        return bool(C.dstar_lite_pqueue_remove_full(
            self._c, key.ptr(), coord.ptr()))

    def contains(self, coord: c_coord):
        return bool(C.dstar_lite_pqueue_contains(self._c, coord.ptr()))

    def is_empty(self):
        return bool(C.dstar_lite_pqueue_is_empty(self._c))

    def ptr(self):
        return self._c

    def __repr__(self):
        return f"c_dstar_lite_pqueue(empty={self.is_empty()})"

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

    def copy(self):
        ptr = C.dstar_lite_pqueue_copy(self._c)
        return c_dstar_lite_pqueue(raw_ptr=ptr, own=True)
