# Use GL
find_package(OpenGL REQUIRED)

#----- Enable Unicode -----
#add_definitions(-DUNICODE -D_UNICODE)

# Preprocessor Definitions
SET( MACROS
	FBXSDK_SHARED
	GLEW_STATIC
	FREEGLUT_STATIC
)
# Can include project name and directory
SET( INCLUDE
	glew
	glm
    libbmpread
	fbxsdk
	FreeGlut
	Graphics
	GUI
	zlib
	Math
)
# Can include project name and absolute path to library
SET( LINK
	# ----- OpenGL & Window -----
	${OPENGL_LIBRARIES}
	glew
	FreeGlut
	# ----- Core Framework -----
	Graphics
	GUI
	Math
	# ----- App Specific -----
	zlib
	libpng
    libbmpread
	${fbxsdk_SOURCE_DIR}/lib/x86/debug/libfbxsdk.lib
)

# Create project -- DO NOT EDIT ANYTHING BELOW THIS LINE --
# mode can be set to DYNAMIC, STATIC, CONSOLE, or WIN32
create_project(CONSOLE "${MACROS}" "${INCLUDE}" "${LINK}")