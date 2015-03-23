cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
    RTGI_OUTPUT_SHADER_RESOURCE_LOADING
	GLEW_STATIC
)
SET( INCLUDE_DIRS
	glfw
	Math
	glm
    libbmpread
    libpng
	glew
)
SET( LINK_LIBS
)

create_project(STATIC "${MACROS}" "${INCLUDE_DIRS}" "${LINK_LIBS}")
