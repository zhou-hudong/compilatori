# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_SOURCE_DIR = /home/user/Downloads/Laboratori_Compilatori/assignment3

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/Downloads/Laboratori_Compilatori/assignment3/build

# Include any dependencies generated for this target.
include CMakeFiles/LoopPass.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/LoopPass.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/LoopPass.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/LoopPass.dir/flags.make

CMakeFiles/LoopPass.dir/LoopPass.cpp.o: CMakeFiles/LoopPass.dir/flags.make
CMakeFiles/LoopPass.dir/LoopPass.cpp.o: /home/user/Downloads/Laboratori_Compilatori/assignment3/LoopPass.cpp
CMakeFiles/LoopPass.dir/LoopPass.cpp.o: CMakeFiles/LoopPass.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Downloads/Laboratori_Compilatori/assignment3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/LoopPass.dir/LoopPass.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/LoopPass.dir/LoopPass.cpp.o -MF CMakeFiles/LoopPass.dir/LoopPass.cpp.o.d -o CMakeFiles/LoopPass.dir/LoopPass.cpp.o -c /home/user/Downloads/Laboratori_Compilatori/assignment3/LoopPass.cpp

CMakeFiles/LoopPass.dir/LoopPass.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LoopPass.dir/LoopPass.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/user/Downloads/Laboratori_Compilatori/assignment3/LoopPass.cpp > CMakeFiles/LoopPass.dir/LoopPass.cpp.i

CMakeFiles/LoopPass.dir/LoopPass.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LoopPass.dir/LoopPass.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/user/Downloads/Laboratori_Compilatori/assignment3/LoopPass.cpp -o CMakeFiles/LoopPass.dir/LoopPass.cpp.s

# Object files for target LoopPass
LoopPass_OBJECTS = \
"CMakeFiles/LoopPass.dir/LoopPass.cpp.o"

# External object files for target LoopPass
LoopPass_EXTERNAL_OBJECTS =

libLoopPass.so: CMakeFiles/LoopPass.dir/LoopPass.cpp.o
libLoopPass.so: CMakeFiles/LoopPass.dir/build.make
libLoopPass.so: CMakeFiles/LoopPass.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/Downloads/Laboratori_Compilatori/assignment3/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library libLoopPass.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LoopPass.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/LoopPass.dir/build: libLoopPass.so
.PHONY : CMakeFiles/LoopPass.dir/build

CMakeFiles/LoopPass.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/LoopPass.dir/cmake_clean.cmake
.PHONY : CMakeFiles/LoopPass.dir/clean

CMakeFiles/LoopPass.dir/depend:
	cd /home/user/Downloads/Laboratori_Compilatori/assignment3/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/Downloads/Laboratori_Compilatori/assignment3 /home/user/Downloads/Laboratori_Compilatori/assignment3 /home/user/Downloads/Laboratori_Compilatori/assignment3/build /home/user/Downloads/Laboratori_Compilatori/assignment3/build /home/user/Downloads/Laboratori_Compilatori/assignment3/build/CMakeFiles/LoopPass.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/LoopPass.dir/depend

