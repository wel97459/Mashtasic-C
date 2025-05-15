cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

find_package(Nanopb REQUIRED)
include(/home/winston/Junk/build/nanopb/extra/FindNanopb.cmake)

file(GLOB proto_sources extern/firmware/src/mesh/generated/meshtastic/*.cpp)
include_directories(${NANOPB_INCLUDE_DIRS})
include_directories(extern/firmware/src/mesh/generated/)
include_directories(include/)

add_library(proto SHARED ${proto_sources})

file(GLOB sources src/*.cpp)
add_executable(bar ${sources} )
target_link_libraries(bar proto nanopb)
