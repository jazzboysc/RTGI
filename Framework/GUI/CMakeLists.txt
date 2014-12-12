cmake_minimum_required( VERSION 2.8 )

# Preprocessor Definitions
SET( MACROS
)
SET( INCLUDE
)
SET( LINK
)
SET( VS_DOTNET_REFERENCES
	System
	System.Data
	System.Drawing
	System.Windows.Forms
	System.Xml
)

create_project(STATIC "${MACROS}" "${INCLUDE}" "${LINK}")
AddDotNetReferences("${VS_DOTNET_REFERENCES}")