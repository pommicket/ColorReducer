cmake_minimum_required(VERSION 3.0)
project(ColorReducer)
set(CMAKE_C_FLAGS "-Wall -std=c89")
set(CMAKE_BUILD_TYPE Release)
set(PROJECT_SRC main.c kmeans.c voronoi.c colorreducer.c audioreducer.c rawreducer.c textreducer.c)
add_subdirectory(utils)
add_executable(${PROJECT_NAME} ${PROJECT_SRC})
target_link_libraries(${PROJECT_NAME} m cr_utils)
