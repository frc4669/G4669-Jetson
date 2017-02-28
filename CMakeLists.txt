cmake_minimum_required(VERSION 2.8)

project(core)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_core core.cpp)

target_link_libraries(cv_core ${OpenCV_LIBS})
