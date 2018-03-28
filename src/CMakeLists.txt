
find_library(curl libcurl.a)

link_directories( C:/lib/curl2/lib )

find_package(Boost 1.66.0 REQUIRED COMPONENTS system filesystem thread )

add_library(logutils logutils.cpp config.cpp)
target_include_directories(logutils PUBLIC ../include)

add_library(downloader downloader.cpp)
target_link_libraries(downloader LINK_PUBLIC curl ${Boost_LIBRARIES} )
target_include_directories(downloader PUBLIC C:/lib/curl2/include ${Boost_INCLUDE_DIR} ../include)

add_executable(client client_main.cpp work_client.cpp work_queue.cpp logutils.cpp)
target_include_directories(client PUBLIC ${Boost_INCLUDE_DIR} ../include C:/lib/curl2/include )
target_link_libraries(client LINK_PUBLIC curl ${Boost_LIBRARIES} logutils)
if (WIN32)
    target_link_libraries(client LINK_PUBLIC ws2_32)
endif()
if (UNIX)
    target_link_libraries(client LINK_PUBLIC pthread)
endif()

add_library(agrp work_queue.cpp logutils.cpp )

add_executable(autogrp main.cpp work_client.cpp work_queue.cpp work_server.cpp)
target_include_directories(autogrp PUBLIC ${Boost_INCLUDE_DIR} ../include )
target_link_libraries(autogrp LINK_PUBLIC downloader ${Boost_LIBRARIES} httpparser fmt logutils )
if (WIN32)
    target_link_libraries(autogrp LINK_PUBLIC wsock32 ws2_32)
endif()
if (UNIX)
    target_link_libraries(autogrp LINK_PUBLIC pthread)
endif()
