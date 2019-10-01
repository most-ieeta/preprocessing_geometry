cmake_minimum_required(VERSION 2.8)
project( MOST )

find_library(GEOS_C geos_c)
find_path(GEOS_INC geos_c.h)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

include_directories("./inc")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(vcp src/vcp_main.cpp)
target_link_libraries(vcp ${GEOS_C})	

add_executable(terminal src/terminal.cpp src/polygon.cpp)
target_link_libraries(terminal ${GEOS_C})	

add_executable(pof2wkt src/pof2wkt.cpp src/polygon.cpp)
target_link_libraries(pof2wkt ${GEOS_C})

add_executable(msimp src/multi_simplifier.cpp src/polygon.cpp src/simplifier.cpp)
target_link_libraries(msimp ${GEOS_C})

add_executable(aligner src/aligner_main.cpp src/polygon.cpp src/aligner.cpp)
target_link_libraries(aligner ${GEOS_C})

add_executable(vcp src/polygon.cpp src/linearsegment.cpp src/vcp_main.cpp)
target_link_libraries(aligner ${GEOS_C})
