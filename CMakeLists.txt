cmake_minimum_required (VERSION 3.20)

project ("Corolib")

set(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fcoroutines")

add_subdirectory("corolib")
add_subdirectory ("examples")

