cmake_minimum_required(VERSION 3.26)
project(pong-gl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)
pkg_check_modules(LIBS REQUIRED glew glfw3 freetype2 opencv4)

add_executable(pong main.cpp)

target_link_libraries(pong ${LIBS_LIBRARIES} opengl32)
target_include_directories(pong PUBLIC ${LIBS_INCLUDE_DIRS})
target_compile_options(pong PUBLIC ${LIBS_CFLAGS})
