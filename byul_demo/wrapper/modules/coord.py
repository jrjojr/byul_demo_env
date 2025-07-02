


from ffi_core import ffi, C
import weakref
from pathlib import Path
import os
from typing import Any

ffi.cdef("""

             typedef struct s_coord* coord;
    typedef struct s_coord { int x ; int y ; } coord_t;
     
    coord    coord_new_full(int x, int y);
     coord    coord_new();
     void     coord_free(coord c);
     unsigned int coord_hash(const coord c);
     int coord_equal(const coord c1, const coord c2);
     coord coord_copy(const coord c);
     flud flud_new_coord(const coord s);
     flud flud_wrap_coord(const coord s);
     int flud_fetch_coord(const flud d, coord *out);
     const coord flud_get_coord(const flud d);
     int flud_set_coord(flud d, const coord s);
     int flud_is_coord(const flud d);
     int     coord_get_x(const coord c);
     void     coord_set_x(coord c, int x);
     int     coord_get_y(const coord c);
     void     coord_set_y(coord c, int y);
     unsigned long long  coord_pack(const coord c);
     coord    coord_unpack(unsigned long long packed);
     void     coord_set(coord c, int x, int y);
     void     coord_fetch(coord c, int* out_x, int* out_y);
     int coord_compare(const coord a, const coord b);
         
    gdouble coord_degree(const coord a, const coord b);         
""")

def coord_pack(c: Any) -> Any:
    return C.coord_pack(c)

def coord_unpack(packed: Any) -> Any:
    return C.coord_unpack(packed)

class c_coord:
    def __init__(self, x=0, y=0, raw_ptr=None):
        if raw_ptr is not None:
            self._c = raw_ptr
            self._own = False
            self._finalizer = None
        else:
            self._c = C.coord_new_full(x, y)
            if not self._c:
                raise MemoryError("coord allocation failed")
            self._own = True
            self._finalizer = weakref.finalize(self, C.coord_free, self._c)

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