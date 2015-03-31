# Use GL
find_package(OpenGL REQUIRED)

#----- Enable Unicode -----
#add_definitions(-DUNICODE -D_UNICODE)

# Preprocessor Definitions
SET( MACROS
	FBXSDK_SHARED
	GLEW_STATIC
)
# Can include project name and directory
SET( INCLUDE
	# ----- OpenGL & Window -----
	glew
	glm
	glut
	# ----- Core Framework -----
	Graphics
	GUI
	Math
	# ----- App Specific -----
	zlib
    libbmpread
)
# Can include project name and absolute path to library
SET( LINK
	# ----- OpenGL & Window -----
	${OPENGL_LIBRARIES}
	glew
	glut
	# ----- Core Framework -----
	Graphics
	GUI
	Math
	# ----- App Specific -----
	zlib
	libpng
    libbmpread
)

# Create project -- DO NOT EDIT ANYTHING BELOW THIS LINE --
# mode can be set to DYNAMIC, STATIC, CONSOLE, or WIN32
create_project(CONSOLE "${MACROS}" "${INCLUDE}" "${LINK}")