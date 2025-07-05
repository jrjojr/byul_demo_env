# ffi_core.py
from cffi import FFI
from pathlib import Path
import os
import platform
import sys

ffi = FFI()

ffi.cdef("""
#define TRUE 1
#define FALSE 0
""")

# --- 플랫폼 구분 및 libroutefinder 로딩 ---
system = platform.system()
root = Path.home() / "byul_demo"

# if system == "Windows":
#     routefinder_name = "libroutefinder.dll"
#     routefinder_path = root / "bin" / routefinder_name

#     os.add_dll_directory(str(routefinder_path.parent))  # 필수

# elif system == "Linux":
#     routefinder_name = "libroutefinder.so"
#     routefinder_path = root / "lib" / routefinder_name

# elif system == "Darwin":
#     routefinder_name = "libroutefinder.dylib"
#     routefinder_path = root / "lib" / routefinder_name

# else:
#     raise RuntimeError(f"❌ 지원되지 않는 플랫폼: {system}")

# # --- libroutefinder 로드 ---
# try:
#     C = ffi.dlopen(str(routefinder_path))
# except OSError as e:
#     print(f"❌ [libroutefinder] 로딩 실패: {routefinder_path}")
#     print(f"→ {e}")
#     raise RuntimeError("libroutefinder 바이너리 확인 필요")

if system == "Windows":
    routefinder_path = root / "bin" / "libroutefinder.dll"

    # --- MinGW DLL 경로 추가 (자동) ---
    mingw_path = Path("C:/msys64/mingw64/bin")  # 별이아빠님의 환경에 맞게
    if mingw_path.exists():
        if hasattr(os, "add_dll_directory"):
            os.add_dll_directory(str(mingw_path))
        else:
            os.environ["PATH"] = str(mingw_path) + os.pathsep + os.environ["PATH"]

    # libroutefinder 경로도 추가
    if hasattr(os, "add_dll_directory"):
        os.add_dll_directory(str(routefinder_path.parent))
    else:
        os.environ["PATH"] = str(routefinder_path.parent) + os.pathsep + os.environ["PATH"]

elif system == "Linux":
    routefinder_path = root / "lib" / "libroutefinder.so"

elif system == "Darwin":
    routefinder_path = root / "lib" / "libroutefinder.dylib"

else:
    raise RuntimeError(f"❌ 지원되지 않는 플랫폼: {system}")

# 라이브러리 로딩
try:
    C = ffi.dlopen(str(routefinder_path))
except OSError as e:
    print(f"❌ [libroutefinder] 로딩 실패: {routefinder_path}")
    print(f"→ {e}")
    raise RuntimeError("libroutefinder 바이너리 또는 의존 DLL 확인 필요")