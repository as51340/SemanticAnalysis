cmake_minimum_required (VERSION 3.13)
project (PpjCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES "*.cpp")

add_executable(semantic ${SOURCES})
