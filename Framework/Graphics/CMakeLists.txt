cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
    RTGI_OUTPUT_SHADER_RESOURCE_LOADING
)
SET( INCLUDE_DIRS
	FreeGlut
	Math
	glm
    libbmpread
	glew
)
SET( LINK_LIBS
)

create_project(STATIC "${MACROS}" "${INCLUDE_DIRS}" "${LINK_LIBS}")
