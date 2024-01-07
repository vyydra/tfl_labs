cmake_minimum_required(VERSION 3.20)
project(lab4)

add_executable(lab4 "src/main.cpp" "src/slr1.cpp")
target_include_directories(lab4 PRIVATE "${PROJECT_SOURCE_DIR}/include")
