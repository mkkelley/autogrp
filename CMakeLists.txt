cmake_minimum_required(VERSION 3.9)
project(autogrp)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib/fmt)
add_subdirectory(lib/http-parser)
add_subdirectory(rsgf_viewer)

link_directories( C:/lib/curl2/lib )

find_package(Boost 1.66.0 REQUIRED COMPONENTS system filesystem thread )

add_executable(autogrp main.cpp work_queue.cpp work_server.cpp work_client.cpp logutils.cpp)
add_library(downloader downloader.cpp)

target_include_directories(autogrp PUBLIC ${Boost_INCLUDE_DIR} include )
target_link_libraries(autogrp LINK_PUBLIC downloader ${Boost_LIBRARIES} ws2_32 wsock32 httpparser fmt )

find_library(curl libcurl.a)
target_link_libraries(downloader LINK_PUBLIC curl ${Boost_LIBRARIES} )
target_include_directories(downloader PUBLIC C:/lib/curl2/include ${Boost_INCLUDE_DIR} include)
