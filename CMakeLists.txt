cmake_minimum_required(VERSION 3.10)
project(MyProject VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

set(CMAKE_MODULE_PATH cmake)
find_package(Nanopb REQUIRED)

file(GLOB proto_buf extern/protobufs/meshtastic/*.proto)
nanopb_generate_cpp(TARGET proto ${proto_buf})


# include_directories(extern/protobufs/meshtastic/*.proto)
include_directories(include/)
include_directories(/usr/local/include/nanopb)

#add_library(proto SHARED ${proto_sources} extern/proto-placeholers/placeholders.cpp)

file(GLOB sources src/*.cpp)
add_executable(bar ${sources} )
target_link_libraries(bar proto protobuf-nanopb)
