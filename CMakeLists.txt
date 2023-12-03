cmake_minimum_required(VERSION 3.27)
project(sdl_cmake)

set(CMAKE_CXX_STANDARD 17)
if (WIN32)
    set(Boost_ROOT "C:\\vcpkg\\installed\\x64-windows\\include")
endif ()

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(Boost REQUIRED COMPONENTS)
find_package(CURL REQUIRED)

add_executable(sdl_cmake main.cpp)

target_include_directories(sdl_cmake PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})
target_link_libraries(sdl_cmake PRIVATE ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
