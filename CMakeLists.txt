cmake_minimum_required( VERSION 2.8 )
# Set the build type.  Options are:
#  Coverage       : w/  debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/  debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/  optimization
#  RelWithDebInfo : w/  debug symbols, w/  optimization
#  MinSizeRel     : w/o debug symbols, w/  optimization, stripped binaries

SET( CMakeScriptRepo "https://github.com/piaoasd123/CMake.git" )
# Grab CMake Script
# Requires Git.exe added to system path
if(WIN32)
	set(githint "$ENV{APPDATA}/../Local/Github/PortableGit*/bin")
	file( TO_CMAKE_PATH ${githint} githint )
	file(GLOB GitPath "${githint}/git.exe")
	set(Terminal $ENV{ComSpec})
endif()
message(STATUS "Git Executable: ${GIT_EXECUTABLE}")
message(STATUS "CMake Script: ${CMakeScriptRepo}")
if(EXISTS ${CMAKE_SOURCE_DIR}/Purify/)
	#execute_process(COMMAND ${Terminal}								#  |
	#	"/c" ${GitPath} "pull" ${CMakeScriptRepo}					#  |
	#	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/Purify/")			#  |
else()																#  |
	#execute_process(COMMAND ${Terminal}								#  |
	#	"/c" ${GitPath} "clone" ${CMakeScriptRepo}					#  |
	#	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")					#  |
endif()
# Set module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Purify")
message(STATUS "CMake Path: ${CMAKE_MODULE_PATH}")

# Include all modules
include( CustomModules )

# --------------------------------------
# DO NOT EDIT ANYTHING ABOVE THIS LINE |
# --------------------------------------

# Add global compile flags and preprocessor definitions
add_definitions(
"-D_CRT_SECURE_NO_WARNINGS"
"/FC")

# Enable Unicode
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /UMBCS /D_UNICODE /DUNICODE")

# Automatic build generation procedure
include( CreateBuild )
#EOF