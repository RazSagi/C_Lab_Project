cmake_minimum_required(VERSION 3.16)
project(C_Lab_Project C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedantic -Wall")

add_executable(C_Lab_Project
        core/assembler.c
        core/helpers.c
)
