# CMakeLists.txt for the test project

cmake_minimum_required(VERSION 3.10)

# Set the project name
project(test_cost_coord_pq)

set(EXTERNAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/../../external)
message(STATUS "EXTERNAL_INCLUDE_DIR = ${EXTERNAL_INCLUDE_DIR}")

# Add the test executable
add_executable(${PROJECT_NAME} ${PROJECT_NAME}.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}/include/routefinder
    ${EXTERNAL_INCLUDE_DIR}
)

# Ensure glib.h can be included
target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/cost_coord_pq
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ${GLIB_CFLAGS_OTHER})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    cost_coord_pq
)

# Add a test to CTest
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE BYUL_STATIC)
