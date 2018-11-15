cmake_minimum_required(VERSION 3.0)
project(kmax)

set(CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-m64 -march=native -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-ftree-vectorize -funroll-loops -Ofast")

# Find dependencies:
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_library(Kmaxdist STATIC kmax_distance.cpp)
add_library(AVLtree STATIC avl_tree.c)
add_library(Distribfunction STATIC distribfunction.cpp)

set(SHARED_LIBS Kmaxdist AVLtree Distribfunction)
#set(SOURCE_FILES main.cpp)

add_executable(main main.cpp)
target_link_libraries(main ${SHARED_LIBS} ${OpenCV_LIBS} )
add_executable(main2 main2.cpp)
target_link_libraries(main2 ${SHARED_LIBS} ${OpenCV_LIBS} )
add_executable(main3 main3.cpp)
target_link_libraries(main3 ${SHARED_LIBS} ${OpenCV_LIBS} )
