cmake_minimum_required(VERSION 3.0)

project(tbb_arena LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


add_executable(main main.cpp)
find_package(TBB REQUIRED)
target_link_libraries(main PUBLIC TBB::tbb)

