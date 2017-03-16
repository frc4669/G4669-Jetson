cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(core)

find_package(OpenCV REQUIRED)
find_library(LibNT ntcore ~/ntcore/arm/ntcore/build/libs/ntcore/static)
find_library(WPIutil wpiutil /home/ubuntu/ntcore/arm/wpiutil/build/libs/wpiutil/static)

include_directories(${OpenCV_INCLUDE_DIRS} ~/ntcore/include ~/ntcore/wpiutil/include)

add_executable(cv_core core.cpp)

target_link_libraries(cv_core ${OpenCV_LIBS} ${LibNT} ${WPIutil})
