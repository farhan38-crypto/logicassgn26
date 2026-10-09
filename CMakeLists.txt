cmake_minimum_required(VERSION 3.16)
project(logic_assignment_1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(logic_tool src/main.cpp)
target_include_directories(logic_tool PRIVATE include)

if(MSVC)
  target_compile_options(logic_tool PRIVATE /W4 /permissive-)
else()
  target_compile_options(logic_tool PRIVATE -Wall -Wextra -Wpedantic)
endif()
