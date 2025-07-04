cmake_minimum_required(VERSION 3.10)

project(test_coord)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# ------------------- 테스트 실행 파일 -------------------
add_executable(${PROJECT_NAME}
    test_coord.cpp
)

# 외부 include 경로 설정
set(EXTERNAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../../external)
message(STATUS "EXTERNAL_INCLUDE_DIR = ${EXTERNAL_INCLUDE_DIR}")

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include/routefinder
    ${EXTERNAL_INCLUDE_DIR}
)

# ------------------- 정적 라이브러리 연결 -------------------
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/coord
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    coord
)

# coord는 STATIC 라이브러리이므로 필요 시 심볼 정의
target_compile_definitions(${PROJECT_NAME} PRIVATE BYUL_STATIC)

# ------------------- CTest -------------------
enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
