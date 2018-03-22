


find_package(Boost 1.66.0 REQUIRED COMPONENTS system filesystem thread )

add_executable(rsgf_viewer main.cpp sgf_parser.cpp Sgf.cpp Sgf.h)
add_executable(calc_debug calc_debug.cpp)
add_executable(minimal minimal.cpp)

target_include_directories(rsgf_viewer PUBLIC ${Boost_INCLUDE_DIR} )
target_include_directories(calc_debug PUBLIC ${Boost_INCLUDE_DIR} )
target_include_directories(minimal PUBLIC ${Boost_INCLUDE_DIR} )

target_link_libraries(rsgf_viewer LINK_PUBLIC ${Boost_LIBRARIES} )
target_link_libraries(calc_debug LINK_PUBLIC ${Boost_LIBRARIES} )
target_link_libraries(minimal LINK_PUBLIC ${Boost_LIBRARIES} )
