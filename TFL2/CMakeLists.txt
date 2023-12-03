cmake_minimum_required(VERSION 3.20)
project(lab2)

add_executable(lab2 "src/main.cpp" "src/regexp.cpp")
target_include_directories(lab2 PRIVATE "${PROJECT_SOURCE_DIR}/include")
