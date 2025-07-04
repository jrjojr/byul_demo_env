cmake_minimum_required(VERSION 3.10)
project(test_routefinder)

# ============================
# Executable 정의
# ============================
add_executable(${PROJECT_NAME}
    ${PROJECT_NAME}.c
)

# ============================
# Include 디렉토리 (routefinder만)
# ============================
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include/routefinder
)

# ============================
# Link 디렉토리
# ============================
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}
    ${CMAKE_BINARY_DIR}/lib
)

# ============================
# 라이브러리 연결 (GLib 직접 링크 금지)
# ============================
target_link_libraries(${PROJECT_NAME} PRIVATE routefinder)

# ============================
# 컴파일 매크로 정의
# ============================
target_compile_definitions(${PROJECT_NAME} PRIVATE
    ${GLIB_CFLAGS_OTHER}
    BYUL_STATIC
)

message(STATUS "${PROJECT_NAME}: ✅ routefinder.dll만 링크하고 glib은 절대 직접 연결하지 않음.")

# ============================
# 테스트 등록
# ============================
enable_testing()
add_test(NAME AllTests COMMAND ${PROJECT_NAME})

