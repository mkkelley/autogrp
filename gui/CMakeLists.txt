

find_package(Qt5Widgets)
if (UNIX)
	find_package(OpenGL)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(agrpgui main.cpp
                       mainwindow.cpp
					   sgf_table_model.cpp
					   analyze_sgf_delegate.cpp
					   queue_view.cpp
					   work_server_wrapper.cpp
					   progress_view.cpp)

target_link_libraries(agrpgui LINK_PUBLIC Qt5::Widgets agrp downloader sgf_parser)
target_include_directories(agrpgui PUBLIC ../src ../include ../rsgf_viewer ${Qt5Widgets_INCLUDE_DIRS})
if (UNIX)
	target_link_libraries(agrpgui LINK_PUBLIC ${OPENGL_LIBRARIES})
endif()

#isn't building on windows SO MUCH FUN
if (WIN32)
	add_custom_command(TARGET agrpgui POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"C:/lib/vcpkg/installed/x64-windows/bin"
	$<TARGET_FILE_DIR:agrpgui>
	)

endif()
