from ffi_core import ffi, C
import weakref

from coord import c_coord
from coord_list import c_coord_list

ffi.cdef("""
typedef struct s_coord_hash coord_hash_t;

// 생성/해제
coord_hash_t* coord_hash_new(void);
void          coord_hash_free(coord_hash_t* hash);
coord_hash_t* coord_hash_copy(const coord_hash_t* original);

// 기본 연산
int   coord_hash_length(const coord_hash_t* hash);
bool  coord_hash_is_empty(const coord_hash_t* hash);
void* coord_hash_get(const coord_hash_t* hash, const coord_t* key);
bool  coord_hash_contains(
    const coord_hash_t* hash, const coord_t* key);

// 설정/수정
void  coord_hash_set(coord_hash_t* hash, 
    const coord_t* key, void* value); 

bool  coord_hash_insert(coord_hash_t* hash, 
    const coord_t* key, void* value); 

// 키는 유지하고 값은 변경한다. 이미 키가 존재하면
bool coord_hash_replace(coord_hash_t* hash, 
    const coord_t* key, void* value);    
    
bool  coord_hash_remove(coord_hash_t* hash, const coord_t* key);
void  coord_hash_clear(coord_hash_t* hash);
void  coord_hash_remove_all(coord_hash_t* hash);

// 비교
bool  coord_hash_equal(const coord_hash_t* a, const coord_hash_t* b);

// 키/값 조회
coord_list_t* coord_hash_keys(const coord_hash_t* hash);
void**        coord_hash_values(
    const coord_hash_t* hash, int* out_count);

// 반복
typedef void (*coord_hash_func)(
    const coord_t* key, void* value, void* user_data);

void  coord_hash_foreach(
    coord_hash_t* hash, coord_hash_func func, void* user_data);

// 변환
coord_list_t* coord_hash_to_list(const coord_hash_t* hash);

// 확장
void coord_hash_export(
    const coord_hash_t* hash,
    coord_list_t* keys_out,
    void** values_out,
    int* count_out);

typedef struct s_coord_hash_iter coord_hash_iter_t;

coord_hash_iter_t* coord_hash_iter_new(
    const coord_hash_t* hash);

bool coord_hash_iter_next(
    coord_hash_iter_t* iter, coord_t** key_out, void** val_out);

void coord_hash_iter_free(
    coord_hash_iter_t* iter);

""")

class c_coord_hash:
    def __init__(self, raw_ptr=None, own=True):
        if raw_ptr is not None:
            self._c = raw_ptr
            self._own = own
        else:
            self._c = C.coord_hash_new()
            if not self._c:
                raise MemoryError("coord_hash allocation failed")
            self._own = True

        self._finalizer = weakref.finalize(
            self, C.coord_hash_free, self._c) if self._own else None

    def __len__(self):
        return C.coord_hash_length(self._c)

    def empty(self):
        return C.coord_hash_is_empty(self._c)

    def get(self, key: c_coord):
        return C.coord_hash_get(self._c, key.ptr())

    def contains(self, key: c_coord):
        return C.coord_hash_contains(self._c, key.ptr())

    def set(self, key: c_coord, value):
        C.coord_hash_set(self._c, key.ptr(), value)

    def insert(self, key: c_coord, value):
        return C.coord_hash_insert(self._c, key.ptr(), value)

    def replace(self, key: c_coord, value):
        return C.coord_hash_replace(self._c, key.ptr(), value)

    def remove(self, key: c_coord):
        return C.coord_hash_remove(self._c, key.ptr())

    def clear(self):
        C.coord_hash_clear(self._c)

    def remove_all(self):
        C.coord_hash_remove_all(self._c)

    def copy(self):
        new_ptr = C.coord_hash_copy(self._c)
        return c_coord_hash(raw_ptr=new_ptr, own=True)

    def equals(self, other):
        if not isinstance(other, c_coord_hash):
            return False
        return C.coord_hash_equal(self._c, other._c)

    def keys(self):
        ptr = C.coord_hash_keys(self._c)
        return c_coord_list(raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def values(self):
        out_count = ffi.new("int*")
        val_ptr = C.coord_hash_values(self._c, out_count)
        count = out_count[0]
        if val_ptr == ffi.NULL:
            return []
        return [val_ptr[i] for i in range(count)]

    def to_list(self):
        ptr = C.coord_hash_to_list(self._c)
        return c_coord_list(raw_ptr=ptr, own=True) if ptr != ffi.NULL else None

    def export(self):
        out_count = ffi.new("int*")
        keys_out = C.coord_list_new()
        values_out = ffi.new("void**[]", 1)  # double pointer array
        C.coord_hash_export(self._c, keys_out, values_out, out_count)
        keys = c_coord_list(raw_ptr=keys_out, own=True)
        values = [values_out[0][i] for i in range(out_count[0])]
        return keys, values

    def foreach(self, func, user_data=None):
        if not callable(func):
            raise TypeError("foreach requires a callable function")

        @ffi.callback("void(const coord_t*, void*, void*)")
        def wrapped_func(c_key, c_val, udata):
            key = c_coord(raw_ptr=c_key)
            func(key, c_val)

        C.coord_hash_foreach(self._c, wrapped_func, ffi.NULL)

    def __iter__(self):
        return c_coord_hash_iter(self)

    def ptr(self):
        return self._c

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
        return f"c_coord_hash(len={len(self)})"

class c_coord_hash_iter:
    def __init__(self, hash_obj: c_coord_hash):
        self._iter = C.coord_hash_iter_new(hash_obj.ptr())
        if not self._iter:
            raise MemoryError("coord_hash_iter allocation failed")

    def __iter__(self):
        return self

    def __next__(self):
        key_ptr = ffi.new("coord_t**")
        val_ptr = ffi.new("void**")
        if not C.coord_hash_iter_next(self._iter, key_ptr, val_ptr):
            self.close()
            raise StopIteration
        return (c_coord(raw_ptr=key_ptr[0]), val_ptr[0])

    def close(self):
        if self._iter:
            C.coord_hash_iter_free(self._iter)
            self._iter = None

    def __del__(self):
        self.close()
