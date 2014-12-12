cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
	FBXSDK_SHARED
)
SET( INCLUDE_DIRS
	fbxsdk
	Math
	Graphics
	FreeGlut
	glm
    libbmpread
	glew
)
SET( LINK_LIBS
)

create_project(STATIC "${MACROS}" "${INCLUDE_DIRS}" "${LINK_LIBS}")
