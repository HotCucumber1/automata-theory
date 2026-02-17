cmake_minimum_required(VERSION 3.29)
project(automata_theory)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(Transform)
add_subdirectory(Minimize)
add_subdirectory(NFA)
add_subdirectory(Grammar)
add_subdirectory(Regular)
add_subdirectory(Lexer)
add_subdirectory(Robot)
add_subdirectory(RecursiveDown)
add_subdirectory(CYK)
