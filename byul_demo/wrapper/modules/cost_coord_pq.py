import weakref

from ffi_core import ffi, C

from coord import c_coord

ffi.cdef("""
typedef struct s_cost_coord_pq cost_coord_pq_t;

// ------------------------ 생성/해제 ------------------------

/// @brief float 우선순위 기반 coord_t* 저장 큐 생성
cost_coord_pq_t* cost_coord_pq_new();

/// @brief 큐 해제
void cost_coord_pq_free(cost_coord_pq_t* pq);

// ------------------------ 삽입/조회 ------------------------

/// @brief (비용, 좌표) 쌍 삽입
void cost_coord_pq_push(cost_coord_pq_t* pq, float cost, coord_t* c);

/// @brief 현재 최소 비용 좌표 조회 (삭제하지 않음)
coord_t* cost_coord_pq_peek(cost_coord_pq_t* pq);

/// @brief 현재 최소 비용 좌표 제거 후 반환
coord_t* cost_coord_pq_pop(cost_coord_pq_t* pq);

/// @brief 최소 비용 값 (float)만 조회
float cost_coord_pq_peek_cost(cost_coord_pq_t* pq);

// ------------------------ 검사/삭제 ------------------------

/// @brief 큐가 비었는지 여부
bool cost_coord_pq_is_empty(cost_coord_pq_t* pq);

/// @brief 해당 좌표가 큐에 존재하는지 확인
bool cost_coord_pq_contains(cost_coord_pq_t* pq, coord_t* c);

/// @brief 기존 좌표를 새로운 비용으로 업데이트
void cost_coord_pq_update(
    cost_coord_pq_t* pq, float old_cost, float new_cost, coord_t* c);

/// @brief 해당 좌표의 비용을 제거 (비용 값은 알아야 함)
bool cost_coord_pq_remove(cost_coord_pq_t* pq, float cost, coord_t* c);

int cost_coord_pq_length(cost_coord_pq_t* pq);
void cost_coord_pq_trim_worst(cost_coord_pq_t* pq, int n);
""")

class c_cost_coord_pq:
    def __init__(self, raw_ptr=None, own=True):
        if raw_ptr:
            self._c = raw_ptr
        else:
            self._c = C.cost_coord_pq_new()
            if not self._c:
                raise MemoryError("cost_coord_pq allocation failed")
        self._own = own
        self._finalizer = weakref.finalize(
            self, C.cost_coord_pq_free, self._c) if own else None

    def push(self, cost: float, coord: c_coord):
        C.cost_coord_pq_push(self._c, cost, coord.ptr())

    def peek(self):
        ptr = C.cost_coord_pq_peek(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def peek_cost(self):
        return C.cost_coord_pq_peek_cost(self._c)

    def pop(self):
        ptr = C.cost_coord_pq_pop(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def contains(self, coord: c_coord):
        return bool(C.cost_coord_pq_contains(self._c, coord.ptr()))

    def update(self, old_cost: float, new_cost: float, coord: c_coord):
        C.cost_coord_pq_update(self._c, old_cost, new_cost, coord.ptr())

    def remove(self, cost: float, coord: c_coord):
        return bool(C.cost_coord_pq_remove(self._c, cost, coord.ptr()))

    def trim_worst(self, n: int):
        C.cost_coord_pq_trim_worst(self._c, n)

    def __len__(self):
        return C.cost_coord_pq_length(self._c)

    def is_empty(self):
        return bool(C.cost_coord_pq_is_empty(self._c))

    def ptr(self):
        return self._c

    def __repr__(self):
        return f"c_cost_coord_pq(len={len(self)})"

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
