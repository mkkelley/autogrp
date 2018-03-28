


find_package(Boost 1.66.0 REQUIRED COMPONENTS system filesystem thread )

add_library(sgf_parser sgf_parser.cpp Sgf.cpp move.cpp)

add_executable(rsgf_viewer main.cpp)

#target_include_directories(rsgf_viewer PUBLIC ${Boost_INCLUDE_DIR} )
target_include_directories(sgf_parser PUBLIC ${Boost_INCLUDE_DIR} )

#target_link_libraries(rsgf_viewer LINK_PUBLIC ${Boost_LIBRARIES} logutils)
target_link_libraries(sgf_parser LINK_PUBLIC ${Boost_LIBRARIES} logutils)
target_link_libraries(rsgf_viewer sgf_parser)
