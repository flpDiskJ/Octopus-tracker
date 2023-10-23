cmake_minimum_required(VERSION 3.8)
project(octapus)

find_package(SDL2 REQUIRED)
find_package(SDL2_TTF REQUIRED)

message(${SDL2_TTF_FOUND})
message(h=${SDL2_TTF_LIBRARIES})


set(SRC ${CMAKE_SOURCE_DIR}/src)


add_executable(octapus
	${SRC}/main.cpp
	${SRC}/headers/tracker.cpp
	${SRC}/headers/util.cpp
)
target_include_directories(
	octapus PUBLIC
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
)
target_link_libraries(
	octapus PUBLIC
	${SDL2_TTF_LIBRARIES}
	${SDL2_LIBRARIES}
	SDL2_ttf
)
configure_file(
	${CMAKE_SOURCE_DIR}/fonts/font.TTF 
	${CMAKE_CURRENT_BINARY_DIR}/font.TTF 
	COPYONLY
)
