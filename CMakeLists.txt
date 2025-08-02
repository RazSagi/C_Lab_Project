cmake_minimum_required(VERSION 3.16)
project(C_Lab_Project C)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)
add_compile_options(-pedantic -Wall)

include_directories(${CMAKE_SOURCE_DIR}/headers)

file(GLOB SRC_FILES "core/*.c")

add_executable(assembler ${SRC_FILES}
        core/preproc.c
        headers/preproc.h
        core/macro_list.c
        headers/macro_list.h
)

