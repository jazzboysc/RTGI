//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "PassInfo.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PassInfo::PassInfo()
	:
	mVAO(0)
{
}
//----------------------------------------------------------------------------
PassInfo::~PassInfo()
{
	glDeleteVertexArrays(1, &mVAO);
}
//----------------------------------------------------------------------------
void PassInfo::Create(GLuint program, Primitive* primitive, bool hasNormal,
	bool hasTCoord, int vertexComponentCount)
{
	// Create VAO for this object.
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

    if( primitive->IB )
    {
        primitive->IB->Bind();
    }
    if( primitive->VB )
    {
        primitive->VB->Bind();
    }

	// Specify vertex attributes.
	if( !hasNormal && !hasTCoord )
	{
		GLint loc = glGetAttribLocation(program, "vPosition");
        if( loc != -1 )
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
        }
	}
	else if( hasNormal && !hasTCoord )
	{
		GLint loc = glGetAttribLocation(program, "vPosition");
        if( loc != -1 )
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE,
                vertexComponentCount*sizeof(float), 0);
        }

		loc = glGetAttribLocation(program, "vNormal");
        if( loc != -1 )
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE,
                vertexComponentCount*sizeof(float), (void*)12);
        }
	}
	else if( hasNormal && hasTCoord )
	{
		GLint loc = glGetAttribLocation(program, "vPosition");
        if( loc != -1 )
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE,
                vertexComponentCount*sizeof(float), 0);
        }

		loc = glGetAttribLocation(program, "vTCoord");
        if( loc != -1 )
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE,
                vertexComponentCount*sizeof(float), (void*)12);
        }

        if( loc != -1 )
        {
            loc = glGetAttribLocation(program, "vNormal");
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE,
                vertexComponentCount*sizeof(float), (void*)20);
        }
	}
	else
	{
		GLint loc = glGetAttribLocation(program, "vPosition");
        if( loc != -1 )
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE,
                vertexComponentCount*sizeof(float), 0);
        }

		loc = glGetAttribLocation(program, "vTCoord");
        if( loc != -1 )
        {
            glEnableVertexAttribArray(loc);
            glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE,
                vertexComponentCount*sizeof(float), (void*)12);
        }
	}

	glBindVertexArray(0);

#ifdef _DEBUG
    GLenum error = glGetError();
	printf("%s\n", gluErrorString(error));
    assert(error == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void PassInfo::Enable()
{
	glBindVertexArray(mVAO);
}
//----------------------------------------------------------------------------
void PassInfo::Disable()
{
	glBindVertexArray(0);
}
//----------------------------------------------------------------------------