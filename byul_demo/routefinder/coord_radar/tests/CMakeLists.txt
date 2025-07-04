# CMakeLists.txt for the test project

cmake_minimum_required(VERSION 3.10)

# Set the project name
project(test_coord_radar)

set(EXTERNAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../../external)
message(STATUS "EXTERNAL_INCLUDE_DIR = ${EXTERNAL_INCLUDE_DIR}")

# Add the test executable
add_executable(${PROJECT_NAME} 
    ${PROJECT_NAME}.cpp
    )

# 컴파일·링크 옵션에 ASan 추가
# win11과 우분투는 다르다

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS 
    "${PROJECT_NAME} Applying ASan for Windows (no leak sanitizer)")

    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -g
        -O1
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
    )
else()
    message(STATUS "${PROJECT_NAME} Applying ASan + LSan for Unix-like")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -fsanitize=leak
        -g
        -O1
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        -fsanitize=address
        -fsanitize=leak
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include/routefinder
    ${EXTERNAL_INCLUDE_DIR}
    )

# Ensure glib.h can be included
target_link_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_BINARY_DIR}/lib
    ${CMAKE_BINARY_DIR}/routefinder/coord_radar
    )

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    coord_radar
    m
    )

# Add a test goal CTest
add_test(NAME AllTests COMMAND ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE BYUL_STATIC)