cmake_minimum_required(VERSION 3.8)
project(octopus)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

message(${SDL2_ttf_FOUND})
message(h=${SDL2_TTF_LIBRARIES})


set(SRC ${CMAKE_SOURCE_DIR}/src)

add_executable(octopus
	${SRC}/main.cpp
	${SRC}/headers/tracker.cpp
	${SRC}/headers/util.cpp
	${SRC}/headers/audioworks.cpp
	${SRC}/headers/sequence_list.cpp
	${SRC}/headers/instrument_properties.cpp
	${SRC}/headers/disk_op.cpp
	${SRC}/headers/sample_edit.cpp
	${SRC}/headers/sdl_keymap.cpp
)
target_include_directories(
	octopus PUBLIC
	${SDL2_TTF_INCLUDE_DIRS}
	${SDL2_INCLUDE_DIRS}
)
target_link_libraries(
	octopus PUBLIC
	${SDL2_TTF_LIBRARIES}
	${SDL2_LIBRARIES}
	SDL2_ttf
)
configure_file(
	${CMAKE_SOURCE_DIR}/fonts/font.TTF
	${CMAKE_CURRENT_BINARY_DIR}/font.TTF
	COPYONLY
)
