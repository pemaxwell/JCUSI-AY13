# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/irobot/RTPVideoExamples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/irobot/RTPVideoExamples

# Include any dependencies generated for this target.
include CMakeFiles/VideoExample.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/VideoExample.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/VideoExample.dir/flags.make

CMakeFiles/VideoExample.dir/VideoStream.cpp.o: CMakeFiles/VideoExample.dir/flags.make
CMakeFiles/VideoExample.dir/VideoStream.cpp.o: VideoStream.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/irobot/RTPVideoExamples/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/VideoExample.dir/VideoStream.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/VideoExample.dir/VideoStream.cpp.o -c /home/irobot/RTPVideoExamples/VideoStream.cpp

CMakeFiles/VideoExample.dir/VideoStream.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VideoExample.dir/VideoStream.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/irobot/RTPVideoExamples/VideoStream.cpp > CMakeFiles/VideoExample.dir/VideoStream.cpp.i

CMakeFiles/VideoExample.dir/VideoStream.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VideoExample.dir/VideoStream.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/irobot/RTPVideoExamples/VideoStream.cpp -o CMakeFiles/VideoExample.dir/VideoStream.cpp.s

CMakeFiles/VideoExample.dir/VideoStream.cpp.o.requires:
.PHONY : CMakeFiles/VideoExample.dir/VideoStream.cpp.o.requires

CMakeFiles/VideoExample.dir/VideoStream.cpp.o.provides: CMakeFiles/VideoExample.dir/VideoStream.cpp.o.requires
	$(MAKE) -f CMakeFiles/VideoExample.dir/build.make CMakeFiles/VideoExample.dir/VideoStream.cpp.o.provides.build
.PHONY : CMakeFiles/VideoExample.dir/VideoStream.cpp.o.provides

CMakeFiles/VideoExample.dir/VideoStream.cpp.o.provides.build: CMakeFiles/VideoExample.dir/VideoStream.cpp.o
.PHONY : CMakeFiles/VideoExample.dir/VideoStream.cpp.o.provides.build

CMakeFiles/VideoExample.dir/VideoExample.cpp.o: CMakeFiles/VideoExample.dir/flags.make
CMakeFiles/VideoExample.dir/VideoExample.cpp.o: VideoExample.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/irobot/RTPVideoExamples/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/VideoExample.dir/VideoExample.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/VideoExample.dir/VideoExample.cpp.o -c /home/irobot/RTPVideoExamples/VideoExample.cpp

CMakeFiles/VideoExample.dir/VideoExample.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VideoExample.dir/VideoExample.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/irobot/RTPVideoExamples/VideoExample.cpp > CMakeFiles/VideoExample.dir/VideoExample.cpp.i

CMakeFiles/VideoExample.dir/VideoExample.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VideoExample.dir/VideoExample.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/irobot/RTPVideoExamples/VideoExample.cpp -o CMakeFiles/VideoExample.dir/VideoExample.cpp.s

CMakeFiles/VideoExample.dir/VideoExample.cpp.o.requires:
.PHONY : CMakeFiles/VideoExample.dir/VideoExample.cpp.o.requires

CMakeFiles/VideoExample.dir/VideoExample.cpp.o.provides: CMakeFiles/VideoExample.dir/VideoExample.cpp.o.requires
	$(MAKE) -f CMakeFiles/VideoExample.dir/build.make CMakeFiles/VideoExample.dir/VideoExample.cpp.o.provides.build
.PHONY : CMakeFiles/VideoExample.dir/VideoExample.cpp.o.provides

CMakeFiles/VideoExample.dir/VideoExample.cpp.o.provides.build: CMakeFiles/VideoExample.dir/VideoExample.cpp.o
.PHONY : CMakeFiles/VideoExample.dir/VideoExample.cpp.o.provides.build

# Object files for target VideoExample
VideoExample_OBJECTS = \
"CMakeFiles/VideoExample.dir/VideoStream.cpp.o" \
"CMakeFiles/VideoExample.dir/VideoExample.cpp.o"

# External object files for target VideoExample
VideoExample_EXTERNAL_OBJECTS =

VideoExample: CMakeFiles/VideoExample.dir/VideoStream.cpp.o
VideoExample: CMakeFiles/VideoExample.dir/VideoExample.cpp.o
VideoExample: CMakeFiles/VideoExample.dir/build.make
VideoExample: CMakeFiles/VideoExample.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable VideoExample"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/VideoExample.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/VideoExample.dir/build: VideoExample
.PHONY : CMakeFiles/VideoExample.dir/build

CMakeFiles/VideoExample.dir/requires: CMakeFiles/VideoExample.dir/VideoStream.cpp.o.requires
CMakeFiles/VideoExample.dir/requires: CMakeFiles/VideoExample.dir/VideoExample.cpp.o.requires
.PHONY : CMakeFiles/VideoExample.dir/requires

CMakeFiles/VideoExample.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/VideoExample.dir/cmake_clean.cmake
.PHONY : CMakeFiles/VideoExample.dir/clean

CMakeFiles/VideoExample.dir/depend:
	cd /home/irobot/RTPVideoExamples && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/irobot/RTPVideoExamples /home/irobot/RTPVideoExamples /home/irobot/RTPVideoExamples /home/irobot/RTPVideoExamples /home/irobot/RTPVideoExamples/CMakeFiles/VideoExample.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/VideoExample.dir/depend

