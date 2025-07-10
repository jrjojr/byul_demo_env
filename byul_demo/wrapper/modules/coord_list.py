from ffi_core import ffi, C
import weakref

from coord import c_coord

ffi.cdef("""
// opaque 구조체 정의
typedef struct s_coord_list coord_list_t;

// 생성/소멸
coord_list_t* coord_list_new(void);
void coord_list_free(coord_list_t* list);
coord_list_t* coord_list_copy(const coord_list_t* list);

// 정보 조회
int coord_list_length(const coord_list_t* list);
bool coord_list_empty(const coord_list_t* list);
const coord_t* coord_list_get(const coord_list_t* list, int index);
const coord_t* coord_list_front(const coord_list_t* list);
const coord_t* coord_list_back(const coord_list_t* list);

// 수정
int coord_list_push_back(coord_list_t* list, const coord_t* c);

/// @brief 리스트의 마지막 요소를 제거하고 반환 (NULL이면 없음)
coord_t* coord_list_pop_back(coord_list_t* list);

/// @brief 리스트의 첫 번째 요소를 제거하고 반환 (NULL이면 없음)
coord_t* coord_list_pop_front(coord_list_t* list);

int coord_list_insert(coord_list_t* list, int index, const coord_t* c);
void coord_list_remove_at(coord_list_t* list, int index);
void coord_list_remove_value(coord_list_t* list, const coord_t* c);
void coord_list_clear(coord_list_t* list);
void coord_list_reverse(coord_list_t* list);


// 포함여부 1이면 포함         
int  coord_list_contains(const coord_list_t* list, const coord_t* c);
         
// index 반환, 없으면 -1         
int  coord_list_find(const coord_list_t* list, const coord_t* c);

// 부분 추출
coord_list_t* coord_list_sublist(const coord_list_t* list, int start, int end);

// 비교
bool coord_list_equals(const coord_list_t* a, const coord_list_t* b);
        
""")

class c_coord_list:
    def __init__(self, raw_ptr=None, own=False):
        if raw_ptr is not None:
            self._c = raw_ptr
            self._own = own
        else:
            self._c = C.coord_list_new()
            if not self._c:
                raise MemoryError("coord_list allocation failed")
            self._own = True

        if own:
            self._finalizer = weakref.finalize(
                self, C.coord_list_free, self._c)
        else:
            self._finalizer = None        

    def __len__(self):
        return C.coord_list_length(self._c)

    def __getitem__(self, index):
        if index < 0 or index >= len(self):
            raise IndexError("coord_list index out of range")
        ptr = C.coord_list_get(self._c, index)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def __iter__(self):
        for i in range(len(self)):
            yield self[i]

    def __contains__(self, item):
        if not isinstance(item, c_coord):
            return False
        return C.coord_list_contains(self._c, item.ptr()) != 0

    def index(self, item):
        if not isinstance(item, c_coord):
            raise TypeError("index() expects a c_coord object")
        return C.coord_list_find(self._c, item.ptr())

    def front(self):
        ptr = C.coord_list_front(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def back(self):
        ptr = C.coord_list_back(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def append(self, coord):
        if not isinstance(coord, c_coord):
            raise TypeError("append expects a c_coord object")
        return C.coord_list_push_back(self._c, coord.ptr())

    # def append(self, item):
    #     """자동 추론 기반 append - coord / list / c_coord_list 모두 처리"""
    #     if isinstance(item, c_coord):
    #         return self.append_coord(item)

    #     elif isinstance(item, c_coord_list):
    #         return self.append_coord_list(item)

    #     elif isinstance(item, (list, tuple)):
    #         return self.append_pylist(item)

    #     else:
    #         raise TypeError(f"append: unsupported type {type(item)}")


    # def append_coord(self, coord):
    #     if not isinstance(coord, c_coord):
    #         raise TypeError("append_coord expects a c_coord object")
    #     return C.coord_list_push_back(self._c, coord.ptr())


    # def append_coord_list(self, coord_list):
    #     if not isinstance(coord_list, c_coord_list):
    #         raise TypeError("append_coord_list expects a c_coord_list object")
    #     for i in range(len(coord_list)):
    #         self.append_coord(coord_list[i])


    # def append_pylist(self, lst):
    #     if not isinstance(lst, (list, tuple)):
    #         raise TypeError("append_pylist expects a list or tuple of c_coord")
    #     for coord in lst:
    #         self.append_coord(coord)

    def pop(self):
        ptr = C.coord_list_pop_back(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def pop_front(self):
        ptr = C.coord_list_pop_front(self._c)
        return c_coord(raw_ptr=ptr) if ptr != ffi.NULL else None

    def insert(self, index, coord):
        if not isinstance(coord, c_coord):
            raise TypeError("insert expects a c_coord object")
        return C.coord_list_insert(self._c, index, coord.ptr())

    def remove_at(self, index):
        C.coord_list_remove_at(self._c, index)

    def remove_value(self, coord):
        if not isinstance(coord, c_coord):
            raise TypeError("remove_value expects a c_coord object")
        C.coord_list_remove_value(self._c, coord.ptr())

    def clear(self):
        C.coord_list_clear(self._c)

    def reverse(self):
        C.coord_list_reverse(self._c)

    def copy(self):
        new_ptr = C.coord_list_copy(self._c)
        return c_coord_list(raw_ptr=new_ptr, own=True)

    def sublist(self, start, end):
        new_ptr = C.coord_list_sublist(self._c, start, end)
        return c_coord_list(raw_ptr=new_ptr, own=True)

    def equals(self, other):
        if not isinstance(other, c_coord_list):
            return False
        return C.coord_list_equals(self._c, other._c)

    def empty(self):
        return C.coord_list_empty(self._c)

    def ptr(self):
        return self._c

    def __str__(self):
        return "[" + ", ".join(str(c) for c in self) + "]"

    def __repr__(self):
        return f"c_coord_list(len={len(self)})"

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

    def to_list(self):
        """c_coord_list → Python list[c_coord]"""
        return [c.copy() for c in self]

    @classmethod
    def from_list(cls, lst):
        """
        Python list[c_coord] → c_coord_list
        """
        clist = cls()
        for c in lst:
            if not isinstance(c, c_coord):
                raise TypeError("from_list() expects only c_coord elements")
            clist.append(c)
        return clist
