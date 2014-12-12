//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Expression_H
#define RTGI_Expression_H


#include "Token.h"

namespace RTGI
{

struct LPoint
{
	glm::vec3 Position;
	bool Draw;
};

class LSystem;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/18/2013
//----------------------------------------------------------------------------
class Expression
{
public:
	Expression();
	~Expression();

	Token* GetLHS();

	void GenerateLPointSequence(std::vector<LPoint>& points, glm::vec3& initPos, 
		glm::vec3& initDir, float len, float rotX, float rotY, float rotZ) const;

private:
	friend class LSystem;
	Token* mLHS;
	std::vector<Token*> mRHS;
};

}

#endif