cmake_minimum_required(VERSION 3.0)

project(cpptest)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

add_executable(cpptest main.cpp)

find_package(Threads REQUIRED)
target_link_libraries(cpptest PUBLIC Threads::Threads)
find_package(TBB REQUIRED)
target_link_libraries(cpptest PUBLIC TBB::tbb)
