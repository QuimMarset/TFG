# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
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

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/quim/TFG/LivenessPass

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/quim/TFG/LivenessPass/build

# Include any dependencies generated for this target.
include Liveness/CMakeFiles/LLVMLiveness.dir/depend.make

# Include the progress variables for this target.
include Liveness/CMakeFiles/LLVMLiveness.dir/progress.make

# Include the compile flags for this target's objects.
include Liveness/CMakeFiles/LLVMLiveness.dir/flags.make

Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o: Liveness/CMakeFiles/LLVMLiveness.dir/flags.make
Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o: ../Liveness/Liveness.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/quim/TFG/LivenessPass/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o"
	cd /home/quim/TFG/LivenessPass/build/Liveness && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/LLVMLiveness.dir/Liveness.o -c /home/quim/TFG/LivenessPass/Liveness/Liveness.cpp

Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/LLVMLiveness.dir/Liveness.i"
	cd /home/quim/TFG/LivenessPass/build/Liveness && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/quim/TFG/LivenessPass/Liveness/Liveness.cpp > CMakeFiles/LLVMLiveness.dir/Liveness.i

Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/LLVMLiveness.dir/Liveness.s"
	cd /home/quim/TFG/LivenessPass/build/Liveness && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/quim/TFG/LivenessPass/Liveness/Liveness.cpp -o CMakeFiles/LLVMLiveness.dir/Liveness.s

Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.requires:

.PHONY : Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.requires

Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.provides: Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.requires
	$(MAKE) -f Liveness/CMakeFiles/LLVMLiveness.dir/build.make Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.provides.build
.PHONY : Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.provides

Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.provides.build: Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o


# Object files for target LLVMLiveness
LLVMLiveness_OBJECTS = \
"CMakeFiles/LLVMLiveness.dir/Liveness.o"

# External object files for target LLVMLiveness
LLVMLiveness_EXTERNAL_OBJECTS =

Liveness/libLLVMLiveness.so: Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o
Liveness/libLLVMLiveness.so: Liveness/CMakeFiles/LLVMLiveness.dir/build.make
Liveness/libLLVMLiveness.so: Liveness/CMakeFiles/LLVMLiveness.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/quim/TFG/LivenessPass/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module libLLVMLiveness.so"
	cd /home/quim/TFG/LivenessPass/build/Liveness && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/LLVMLiveness.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Liveness/CMakeFiles/LLVMLiveness.dir/build: Liveness/libLLVMLiveness.so

.PHONY : Liveness/CMakeFiles/LLVMLiveness.dir/build

Liveness/CMakeFiles/LLVMLiveness.dir/requires: Liveness/CMakeFiles/LLVMLiveness.dir/Liveness.o.requires

.PHONY : Liveness/CMakeFiles/LLVMLiveness.dir/requires

Liveness/CMakeFiles/LLVMLiveness.dir/clean:
	cd /home/quim/TFG/LivenessPass/build/Liveness && $(CMAKE_COMMAND) -P CMakeFiles/LLVMLiveness.dir/cmake_clean.cmake
.PHONY : Liveness/CMakeFiles/LLVMLiveness.dir/clean

Liveness/CMakeFiles/LLVMLiveness.dir/depend:
	cd /home/quim/TFG/LivenessPass/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/quim/TFG/LivenessPass /home/quim/TFG/LivenessPass/Liveness /home/quim/TFG/LivenessPass/build /home/quim/TFG/LivenessPass/build/Liveness /home/quim/TFG/LivenessPass/build/Liveness/CMakeFiles/LLVMLiveness.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Liveness/CMakeFiles/LLVMLiveness.dir/depend

