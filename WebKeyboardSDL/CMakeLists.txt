cmake_minimum_required(VERSION 3.0.0)
project(Controller VERSION 0.1.0)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

include(CTest)
enable_testing()

find_package(SDL2 REQUIRED)
find_package(SDL2_net REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})

add_executable(Controller main.cpp)

target_link_libraries(Controller SDL2::Main SDL2::Net)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
