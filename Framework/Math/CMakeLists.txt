cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
)
SET( INCLUDE_DIRS
	glew
	glm
)
SET( LINK_LIBS
)

create_project(STATIC "${MACROS}" "${INCLUDE_DIRS}" "${LINK_LIBS}")
