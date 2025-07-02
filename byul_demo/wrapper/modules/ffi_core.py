# ffi_core.py
from cffi import FFI
from pathlib import Path
import os
import platform
import sys

from ctypes.util import find_library

ffi = FFI()

ffi.cdef("""
#define TRUE 1
#define FALSE 0
         
typedef int gint;
typedef int guint;
typedef int gboolean;
typedef const void* gconstpointer;
typedef void* gpointer;
         
typedef float gfloat;
         
typedef double gdouble;

typedef struct _GQueue GQueue;

typedef struct _GHashTableIter GHashTableIter;
         
typedef struct _GTree GTree;         

typedef void (*GDestroyNotify)(void*);

typedef gint            (*GCompareDataFunc)     (gconstpointer  a,
                                                 gconstpointer  b,
						 gpointer       user_data);         
         
typedef gint            (*GCompareFunc)         (gconstpointer  a,
                                                 gconstpointer  b);         
         
typedef struct s_flud* flud;
""")

# --- 플랫폼 구분 및 libroutefinder 로딩 ---
system = platform.system()
root = Path.home() / "byul_demo"

if system == "Windows":
    routefinder_name = "libroutefinder.dll"
    routefinder_path = root / "bin" / routefinder_name

    glib_path = "C:/msys64/clang64/bin/libglib-2.0-0.dll"    

    os.add_dll_directory(str(routefinder_path.parent))  # 필수
    os.add_dll_directory("C:/msys64/clang64/bin")        # GLib DLL 탐색

elif system == "Linux":
    routefinder_name = "libroutefinder.so"
    routefinder_path = root / "lib" / routefinder_name

    glib_path = find_library("glib-2.0")    

elif system == "Darwin":
    routefinder_name = "libroutefinder.dylib"
    routefinder_path = root / "lib" / routefinder_name

    glib_path = find_library("glib-2.0")    

else:
    raise RuntimeError(f"❌ 지원되지 않는 플랫폼: {system}")

# --- libroutefinder 로드 ---
try:
    C = ffi.dlopen(str(routefinder_path))
except OSError as e:
    print(f"❌ [libroutefinder] 로딩 실패: {routefinder_path}")
    print(f"→ {e}")
    raise RuntimeError("libroutefinder 바이너리 확인 필요")

# --- GLib 로딩 ---
if not glib_path:
    raise RuntimeError("GLib (glib-2.0) 라이브러리를 찾을 수 없습니다.")

try:
    C_glib = ffi.dlopen(glib_path)
except OSError as e:
    print(f"❌ [GLib] 로딩 실패: {glib_path}")
    print(f"→ {e}")
    raise RuntimeError("GLib DLL 또는 환경 설정을 확인하세요.")