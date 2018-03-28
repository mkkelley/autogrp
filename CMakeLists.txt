cmake_minimum_required(VERSION 3.9)
project(autogrp)

set(CMAKE_CXX_STANDARD 17)

set(BOOST_ROOT C:/lib/vcpkg/installed/x64-windows)
add_subdirectory(lib/fmt)
add_subdirectory(lib/http-parser)
add_subdirectory(rsgf_viewer)
add_subdirectory(src)
add_subdirectory(gui)

