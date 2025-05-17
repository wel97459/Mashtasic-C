cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0.0)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/extern/nanopb/extra)
find_package(Nanopb REQUIRED)

file(GLOB proto_files "${CMAKE_CURRENT_SOURCE_DIR}/extern/protobufs/meshtastic/*.proto")

nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS RELPATH "${CMAKE_CURRENT_SOURCE_DIR}/extern/protobufs/" ${proto_files})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

include_directories(include/)
include_directories(./)
include_directories(/usr/local/include/nanopb)
set(CMAKE_C_COMPILER "g++")
add_library(meshtastic SHARED ${PROTO_SRCS} extern/proto-placeholers/placeholders.cpp)

file(GLOB sources src/*.cpp)
add_executable(bar ${sources} )
target_link_libraries(bar meshtastic protobuf-nanopb)
