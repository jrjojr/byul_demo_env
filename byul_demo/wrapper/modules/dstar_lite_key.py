import weakref

from ffi_core import ffi, C

ffi.cdef("""
typedef struct s_dstar_lite_key {
    float k1;
    float k2;
} dstar_lite_key_t;

// ------------------------ 생성/복사/해제 ------------------------

dstar_lite_key_t* dstar_lite_key_new(void);

dstar_lite_key_t* dstar_lite_key_new_full(float k1, float k2);

dstar_lite_key_t* dstar_lite_key_copy(const dstar_lite_key_t* key);

void dstar_lite_key_free(dstar_lite_key_t* key);

// ------------------------ 비교 함수 ------------------------

/// @brief 키가 동등한지 확인 (float 오차 허용)
/// @return true = 거의 같다
bool dstar_lite_key_equal(const dstar_lite_key_t* dsk0,
                                   const dstar_lite_key_t* dsk1);
                                   
/**
 * @brief D* Lite 키 비교 함수.
 *
 * 두 개의 키를 비교하여 정렬 우선순위를 결정합니다.
 * 우선적으로 k1 값을 비교하고, 동일한 경우 k2 값을 비교합니다.
 *
 * @note 두 인자는 모두 NULL이 아니어야 하며, NULL 확인은 호출자가 직접 수행해야 합니다.
 *
 * @param dsk0 비교 대상 키 1 (비교의 왼쪽 피연산자)
 * @param dsk1 비교 대상 키 2 (비교의 오른쪽 피연산자)
 * @return 음수: dsk0 < dsk1  
 *         0: 두 키가 동일  
 *         양수: dsk0 > dsk1
 */
int dstar_lite_key_compare(const dstar_lite_key_t* dsk0,
                                    const dstar_lite_key_t* dsk1);

/// @brief 키의 해시 값 계산 (hash map용)
unsigned int dstar_lite_key_hash(const dstar_lite_key_t* key);

""")

class c_dstar_lite_key:
    def __init__(self, k1=0.0, k2=0.0, raw_ptr=None, own=False):
        if raw_ptr:
            self._c = raw_ptr
            self._own = own
        else:
            self._c = C.dstar_lite_key_new_full(k1, k2)
            if not self._c:
                raise MemoryError("dstar_lite_key allocation failed")
            self._own = True

        if own:
            self._finalizer = weakref.finalize(
                self, C.dstar_lite_key_free, self._c)
        else:
            self._finalizer = None        

    def copy(self):
        ptr = C.dstar_lite_key_copy(self._c)
        return c_dstar_lite_key(raw_ptr=ptr, own=True)

    def equal(self, other: 'c_dstar_lite_key'):
        return bool(C.dstar_lite_key_equal(self._c, other._c))

    def compare(self, other: 'c_dstar_lite_key'):
        return C.dstar_lite_key_compare(self._c, other._c)

    def __eq__(self, other):
        return isinstance(other, c_dstar_lite_key) and self.equal(other)

    def __lt__(self, other):
        return self.compare(other) < 0

    def __gt__(self, other):
        return self.compare(other) > 0

    def __hash__(self):
        return C.dstar_lite_key_hash(self._c)

    def to_tuple(self):
        return (self.k1, self.k2)

    @property
    def k1(self):
        return float(self._c.k1)

    @property
    def k2(self):
        return float(self._c.k2)

    def ptr(self):
        return self._c

    def __repr__(self):
        return f"c_dstar_lite_key(k1={self.k1:.6f}, k2={self.k2:.6f})"

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
