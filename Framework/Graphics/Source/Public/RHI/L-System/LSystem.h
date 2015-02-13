//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_LSystem_H
#define RTGI_LSystem_H

#include "RefObject.h"
#include "Expression.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/18/2013
//----------------------------------------------------------------------------
class LSystem : public RefObject
{
public:
	LSystem();
	~LSystem();

	// Create a L-system object from file.
	bool LoadFromFile(const std::string& fileName);

	Expression* GetStartExpression();
	Expression* GenerateIteration(Expression* exp);

	void GetParams(int& len, int& iter, float& rotX, float& rotY, 
		float& rotZ) const;

private:
	Expression* ParseExpression(const std::string& expression);
	Expression* FindVariableByName(const std::string& name);

	Expression* mStart;
	std::vector<Expression*> mVariables;

	int mLen;
	int mIter;
	float mRotX, mRotY, mRotZ;
};

typedef RefPointer<LSystem> LSystemPtr;

}

#endif