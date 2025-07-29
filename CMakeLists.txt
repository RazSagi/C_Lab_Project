cmake_minimum_required(VERSION 3.31)
project(C_Lab_Project C)

set(CMAKE_C_STANDARD 90)

add_executable(C_Lab_Project main.c
        assembler.c
        assembler.h
        assembler.c)
