from ffi_core import ffi, C
import weakref

ffi.cdef("""
    typedef struct s_coord coord_t;
    
    coord_t*    coord_new_full(int x, int y);
    coord_t*    coord_new();
    void     coord_free(coord_t* c);
    coord_t* coord_copy(const coord_t* c);

    unsigned int coord_hash(const coord_t* c);
    int coord_equal(const coord_t* c1, const coord_t* c2);
    int      coord_compare(const coord_t* c1, const coord_t* c2);         

    int     coord_get_x(const coord_t* c);
    void     coord_set_x(coord_t* c, int x);
    int     coord_get_y(const coord_t* c);
    void     coord_set_y(coord_t* c, int y);
    void     coord_set(coord_t* c, int x, int y);
    void     coord_fetch(coord_t* c, int* out_x, int* out_y);

    // 유클리드 거리 계산
    float      coord_distance(const coord_t* a, const coord_t* b);

    int coord_manhattan_distance(const coord_t* a, const coord_t* b);

    // 360도 반환 좌표들간의 각도를...
    double   coord_degree(const coord_t* a, const coord_t* b);
         
""")

class c_coord:
    def __init__(self, x=0, y=0, raw_ptr=None, own=False):
        if raw_ptr is not None:
            self._c = raw_ptr
            self._own = own
            self._finalizer = None
        else:
            self._c = C.coord_new_full(x, y)
            if not self._c:
                raise MemoryError("coord allocation failed")
            self._own = True

        if own:
            self._finalizer = weakref.finalize(self, C.coord_free, self._c)
        else:
            self._finalizer = None            

    @property
    def x(self):
        return C.coord_get_x(self._c)

    @x.setter
    def x(self, value):
        C.coord_set_x(self._c, value)

    @property
    def y(self):
        return C.coord_get_y(self._c)

    @y.setter
    def y(self, value):
        C.coord_set_y(self._c, value)

    def copy(self):
        c= c_coord(raw_ptr=C.coord_copy(self._c))
        c._own = True
        c._finalizer = weakref.finalize(c, C.coord_free, c.ptr())
        return c

    def distance(self, other:'c_coord'):
        # // 유클리드 거리 계산
        # float      coord_distance(const coord_t* a, const coord_t* b);
        return C.coord_distance(self.ptr(), other.ptr())

    def manhattan_distance(self, other:'c_coord'):
        # int coord_manhattan_distance(const coord_t* a, const coord_t* b);
        return C.coord_manhattan_distance(self.ptr(), other.ptr())

    def degree(self, other):
        return C.coord_degree(self._c, other._c)

    def __eq__(self, other):
        return C.coord_equal(self._c, other._c) != 0

    def __lt__(self, other):
        return C.coord_compare(self._c, other._c) < 0

    def __ge__(self, other):
        return C.coord_compare(self._c, other._c) >= 0

    def __hash__(self):
        return C.coord_hash(self._c)

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

    def __add__(self, other):
        return c_coord(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return c_coord(self.x - other.x, self.y - other.y)

    def __str__(self):
        return f"c_coord(x={self.x}, y={self.y})"

    def __repr__(self):
        return str(self)

    def to_tuple(self):
        return (self.x, self.y)

    def ptr(self):
        return self._c

    @staticmethod
    def from_tuple(t: tuple):
        return c_coord(*t)
    
def test_coord_basic():
    a = c_coord(3, 4)
    b = c_coord(6, 8)
    
    print("a:", a)
    print("b:", b)

    assert a.to_tuple() == (3, 4)
    assert b.to_tuple() == (6, 8)
    assert a + b == c_coord(9, 12)
    assert b - a == c_coord(3, 4)

    print("distance(a, b):", a.distance(b))
    print("manhattan(a, b):", a.manhattan_distance(b))
    print("degree(a, b):", a.degree(b))


def test_coord_eq_hash():
    c1 = c_coord(1, 2)
    c2 = c_coord(1, 2)
    c3 = c_coord(2, 1)

    assert c1 == c2
    assert c1 != c3
    coord_set = {c1.copy(), c2.copy(), c3.copy()}
    assert len(coord_set) == 2


def test_coord_copy_and_free():
    c = c_coord(10, 20)
    c2 = c.copy()
    assert c == c2
    c.close()
    c2.close()


def test_from_tuple():
    t = (7, 9)
    c = c_coord.from_tuple(t)
    assert c.to_tuple() == t
    print("from_tuple:", c)


if __name__ == "__main__":
    test_coord_basic()
    test_coord_eq_hash()
    test_coord_copy_and_free()
    test_from_tuple()
    print("✅ c_coord 모든 테스트 통과")

