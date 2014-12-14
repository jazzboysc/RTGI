cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
_GLFW_USE_OPENGL
_GLFW_WIN32
_GLFW_WGL
GLEW_STATIC
)
SET( INCLUDE
)
SET( LINK
)

create_project(STATIC "${MACROS}" "${INCLUDE}" "${LINK}")