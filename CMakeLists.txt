cmake_minimum_required(VERSION 3.9)
project(autogrp)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(rabbitmq-c-0.8.0)

set( Rabbitmqc_INCLUDE_DIR C:/users/purti/clionprojects/autogrp/rabbitmq-c-0.8.0/librabbitmq)
set( Rabbitmqc_LIBRARY C:/users/purti/clionprojects/autogrp/cmake-build-debug/rabbitmq-c-0.8.0/librabbitmq/librabbitmq.4.dll)
add_subdirectory(SimpleAmqpClient-master)

add_subdirectory(rsgf_viewer)

link_directories( C:/lib/curl2/lib )

find_package(Boost 1.66.0 REQUIRED COMPONENTS system filesystem thread )

add_executable(autogrp main.cpp)
add_executable(downloader downloader.cpp)

target_include_directories(autogrp PUBLIC SimpleAmqpClient-master/src ${Boost_INCLUDE_DIR} )
target_link_libraries(autogrp LINK_PUBLIC SimpleAmqpClient rabbitmq ${Boost_LIBRARIES} )

find_library(curl libcurl.a)
target_link_libraries(downloader LINK_PUBLIC curl ${Boost_LIBRARIES} )
target_include_directories(downloader PUBLIC C:/lib/curl2/include ${Boost_INCLUDE_DIR} C:/lib/rapidjson-master/include)
