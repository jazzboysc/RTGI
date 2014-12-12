# Use GL
find_package(OpenGL REQUIRED)

#----- Enable Unicode -----
#add_definitions(-DUNICODE -D_UNICODE)

# Preprocessor Definitions
SET( MACROS
	FBXSDK_SHARED
	GLEW_STATIC
	FREEGLUT_STATIC
	GLM_FORCE_PURE
)

# Can include project name and directory
SET( INCLUDE
	# ----- OpenGL & Window -----
	glew
	# ----- Core Framework -----
	GUI
	Math
	glm
	Graphics	
	# ----- App Specific -----
    libbmpread
	fbxsdk
	zlib
	FreeGlut
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

SET( VS_DOTNET_REFERENCES
	System
	System.Data
	System.Drawing
	System.Windows.Forms
)

# Create project -- DO NOT EDIT ANYTHING BELOW THIS LINE --
# mode can be set to DYNAMIC, STATIC, CONSOLE, or WIN32
create_project(CONSOLE "${MACROS}" "${INCLUDE}" "${LINK}")
AddDotNetReferences("${VS_DOTNET_REFERENCES}")