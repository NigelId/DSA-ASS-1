cmake_minimum_required(VERSION 3.12..3.15)

project(manh LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${CMAKE_PROJECT_NAME} main.cpp VectorStore.cpp VectorStore.h main.h utils.h)

