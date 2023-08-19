cmake_minimum_required(VERSION 3.25)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_executable(cli ./cli/cssa)
add_executable(parser core/opjp/parser.c)