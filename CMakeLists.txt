

#BUILD-------------------------------------------------------------------------------------------->
# cmake -DCMAKE_BUILD_TYPE=Debug -S . -B build
# VERBOSE=1 cmake --build ./build
# bear -- cmake --build ./build
# cmake --build ./build && ./build/hash_table.out
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

#COMPILE_FLAGS------------------------------------------------------------------------------------>

set(SANITIZER_FLAGS "-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,\
integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,\
shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")

set(CMAKE_CXX_FLAGS_DEBUG "-D NDEBUG -O2")

set(CMAKE_CXX_FLAGS_DEBUG "-D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations       \
-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported              \
-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security                       \
-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual              \
-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel   \
-Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override       \
-Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros         \
-Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs                     \
-Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging           \
-fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SANITIZER_FLAGS}")
set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} ${SANITIZER_FLAGS}")

#------------------------------------------------------------------------------------------------->

set(SRCS src/hash_functions.cpp src/hash_tables/version_1.cpp src/data_functions.cpp)
add_library(lclib STATIC ${SRCS})
target_include_directories(lclib PUBLIC "inc")
add_executable(hash_table.out main.cpp)
target_link_libraries(hash_table.out lclib)
