cmake_minimum_required (VERSION 3.13)
project (PpjCompiler)

include_directories(include)

file(GLOB SOURCES "*.cpp")

add_executable(semantic ${SOURCES})
