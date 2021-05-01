cmake_minimum_required(VERSION 3.16.3)
project(IOS2 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -Werror -pedantic -pthread")
add_executable(IOS2 main.c)