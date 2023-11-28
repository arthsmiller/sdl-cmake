cmake_minimum_required(VERSION 3.27)
project(sdl_cmake)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_TOOLCHAIN_FILE "C:\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
#set(CMAKE_PREFIX_PATH "C:\\vcpkg\\packages\\sdl2_x64-windows", "C:\\vcpkg\\packages\\boost_x64-windows")
if (WIN32)
    set(Boost_ROOT "C:\\vcpkg\\installed\\x64-windows\\include")
endif ()

#include("C:\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")

find_package(SDL2 REQUIRED)
find_package(Boost REQUIRED COMPONENTS)

add_executable(sdl_cmake main.cpp)

target_include_directories(sdl_cmake PRIVATE ${SDL2_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_libraries(sdl_cmake PRIVATE ${SDL2_LIBRARIES} ${Boost_LIBRARIES})
