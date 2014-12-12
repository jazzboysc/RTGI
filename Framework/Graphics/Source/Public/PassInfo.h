//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PassInfo_H
#define RTGI_PassInfo_H



namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/09/2014
//----------------------------------------------------------------------------
class PassInfo
{
public:
	PassInfo();
	~PassInfo();

	void Create(GLuint program, GLuint vbo, GLuint ibo, bool hasNormal, 
		bool hasTCoord, int vertexComponentCount);

	void Enable();
	void Disable();

private:
	GLuint mVAO;
};

}

#endif