

#BUILD-------------------------------------------------------------------------------------------->
# cmake -DCMAKE_BUILD_TYPE=Release -S . -B build
# VERBOSE=1 cmake --build ./build
# bear -- cmake --build ./build

#PRESET------------------------------------------------------------------------------------------->
cmake_minimum_required(VERSION 3.11)

set(ENV{CC} "gcc")
set(ENV{CXX} "g++")

project("hash_table")

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)

message(STATUS "C standard " ${CMAKE_C_STANDARD})
message(STATUS "C++ standard " ${CMAKE_CXX_STANDARD})
#------------------------------------------------------------------------------------------------->



SET(SRCS src/hash_tables/version_1.cpp)
add_library(lclib STATIC ${SRCS})
target_include_directories(lclib PUBLIC "inc")
add_executable(hash_table.out main.cpp)
target_link_libraries(hash_table.out lclib)
