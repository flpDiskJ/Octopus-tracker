# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/stefonzo/Programs/Octapus-tracker

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/stefonzo/Programs/Octapus-tracker

# Include any dependencies generated for this target.
include CMakeFiles/octapus.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/octapus.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/octapus.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/octapus.dir/flags.make

CMakeFiles/octapus.dir/src/main.cpp.o: CMakeFiles/octapus.dir/flags.make
CMakeFiles/octapus.dir/src/main.cpp.o: src/main.cpp
CMakeFiles/octapus.dir/src/main.cpp.o: CMakeFiles/octapus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/stefonzo/Programs/Octapus-tracker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/octapus.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/octapus.dir/src/main.cpp.o -MF CMakeFiles/octapus.dir/src/main.cpp.o.d -o CMakeFiles/octapus.dir/src/main.cpp.o -c /home/stefonzo/Programs/Octapus-tracker/src/main.cpp

CMakeFiles/octapus.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/octapus.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stefonzo/Programs/Octapus-tracker/src/main.cpp > CMakeFiles/octapus.dir/src/main.cpp.i

CMakeFiles/octapus.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/octapus.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stefonzo/Programs/Octapus-tracker/src/main.cpp -o CMakeFiles/octapus.dir/src/main.cpp.s

CMakeFiles/octapus.dir/src/headers/tracker.cpp.o: CMakeFiles/octapus.dir/flags.make
CMakeFiles/octapus.dir/src/headers/tracker.cpp.o: src/headers/tracker.cpp
CMakeFiles/octapus.dir/src/headers/tracker.cpp.o: CMakeFiles/octapus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/stefonzo/Programs/Octapus-tracker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/octapus.dir/src/headers/tracker.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/octapus.dir/src/headers/tracker.cpp.o -MF CMakeFiles/octapus.dir/src/headers/tracker.cpp.o.d -o CMakeFiles/octapus.dir/src/headers/tracker.cpp.o -c /home/stefonzo/Programs/Octapus-tracker/src/headers/tracker.cpp

CMakeFiles/octapus.dir/src/headers/tracker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/octapus.dir/src/headers/tracker.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stefonzo/Programs/Octapus-tracker/src/headers/tracker.cpp > CMakeFiles/octapus.dir/src/headers/tracker.cpp.i

CMakeFiles/octapus.dir/src/headers/tracker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/octapus.dir/src/headers/tracker.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stefonzo/Programs/Octapus-tracker/src/headers/tracker.cpp -o CMakeFiles/octapus.dir/src/headers/tracker.cpp.s

CMakeFiles/octapus.dir/src/headers/util.cpp.o: CMakeFiles/octapus.dir/flags.make
CMakeFiles/octapus.dir/src/headers/util.cpp.o: src/headers/util.cpp
CMakeFiles/octapus.dir/src/headers/util.cpp.o: CMakeFiles/octapus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/stefonzo/Programs/Octapus-tracker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/octapus.dir/src/headers/util.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/octapus.dir/src/headers/util.cpp.o -MF CMakeFiles/octapus.dir/src/headers/util.cpp.o.d -o CMakeFiles/octapus.dir/src/headers/util.cpp.o -c /home/stefonzo/Programs/Octapus-tracker/src/headers/util.cpp

CMakeFiles/octapus.dir/src/headers/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/octapus.dir/src/headers/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stefonzo/Programs/Octapus-tracker/src/headers/util.cpp > CMakeFiles/octapus.dir/src/headers/util.cpp.i

CMakeFiles/octapus.dir/src/headers/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/octapus.dir/src/headers/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stefonzo/Programs/Octapus-tracker/src/headers/util.cpp -o CMakeFiles/octapus.dir/src/headers/util.cpp.s

CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o: CMakeFiles/octapus.dir/flags.make
CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o: src/headers/audioworks.cpp
CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o: CMakeFiles/octapus.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/stefonzo/Programs/Octapus-tracker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o -MF CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o.d -o CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o -c /home/stefonzo/Programs/Octapus-tracker/src/headers/audioworks.cpp

CMakeFiles/octapus.dir/src/headers/audioworks.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/octapus.dir/src/headers/audioworks.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/stefonzo/Programs/Octapus-tracker/src/headers/audioworks.cpp > CMakeFiles/octapus.dir/src/headers/audioworks.cpp.i

CMakeFiles/octapus.dir/src/headers/audioworks.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/octapus.dir/src/headers/audioworks.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/stefonzo/Programs/Octapus-tracker/src/headers/audioworks.cpp -o CMakeFiles/octapus.dir/src/headers/audioworks.cpp.s

# Object files for target octapus
octapus_OBJECTS = \
"CMakeFiles/octapus.dir/src/main.cpp.o" \
"CMakeFiles/octapus.dir/src/headers/tracker.cpp.o" \
"CMakeFiles/octapus.dir/src/headers/util.cpp.o" \
"CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o"

# External object files for target octapus
octapus_EXTERNAL_OBJECTS =

octapus: CMakeFiles/octapus.dir/src/main.cpp.o
octapus: CMakeFiles/octapus.dir/src/headers/tracker.cpp.o
octapus: CMakeFiles/octapus.dir/src/headers/util.cpp.o
octapus: CMakeFiles/octapus.dir/src/headers/audioworks.cpp.o
octapus: CMakeFiles/octapus.dir/build.make
octapus: /usr/lib64/libSDL2main.a
octapus: /usr/lib64/libSDL2-2.0.so.0.2800.5
octapus: CMakeFiles/octapus.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/stefonzo/Programs/Octapus-tracker/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable octapus"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/octapus.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/octapus.dir/build: octapus
.PHONY : CMakeFiles/octapus.dir/build

CMakeFiles/octapus.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/octapus.dir/cmake_clean.cmake
.PHONY : CMakeFiles/octapus.dir/clean

CMakeFiles/octapus.dir/depend:
	cd /home/stefonzo/Programs/Octapus-tracker && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/stefonzo/Programs/Octapus-tracker /home/stefonzo/Programs/Octapus-tracker /home/stefonzo/Programs/Octapus-tracker /home/stefonzo/Programs/Octapus-tracker /home/stefonzo/Programs/Octapus-tracker/CMakeFiles/octapus.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/octapus.dir/depend
