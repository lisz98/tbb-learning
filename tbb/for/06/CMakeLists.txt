cmake_minimum_required(VERSION 3.0)

project(main)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

add_executable(main main.cpp)

find_package(Threads REQUIRED)
target_link_libraries(main PUBLIC Threads::Threads)
find_package(TBB REQUIRED)
target_link_libraries(main PUBLIC TBB::tbb)
