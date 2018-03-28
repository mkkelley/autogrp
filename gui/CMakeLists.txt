
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
find_package(Qt5Widgets)

add_executable(agrpgui main.cpp mainwindow.cpp)

target_link_libraries(agrpgui LINK_PUBLIC Qt5::Widgets agrp downloader sgf_parser)
target_include_directories(agrpgui PUBLIC ../src ../include ../rsgf_viewer)
