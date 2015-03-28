//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Expression.h"

using namespace RTGI;

struct TurtleState
{
	glm::vec3 Position;
	glm::vec3 Direction;
};

//----------------------------------------------------------------------------
Expression::Expression()
{
	mLHS = new Token;
}
//----------------------------------------------------------------------------
Expression::~Expression()
{
	delete mLHS;
	for( int i = 0; i < (int)mRHS.size(); ++i )
	{
		delete mRHS[i];
	}
}
//----------------------------------------------------------------------------
Token* Expression::GetLHS()
{
	return mLHS;
}
//----------------------------------------------------------------------------
void Expression::GenerateLPointSequence(std::vector<LPoint>& points, 
	glm::vec3& initPos, glm::vec3& initDir, float len, float rotX, float rotY, 
	float rotZ) const
{
	glm::vec3 curPos = initPos;
	glm::vec3 curDir = initDir;
	std::vector<TurtleState> stateStack;

	LPoint curLPoint;
	curLPoint.Position = curPos;
	//curLPoint.Draw = true;
	//points.push_back(curLPoint);
	for( unsigned int i = 0; i < mRHS.size(); ++i )
	{
		if( mRHS[i]->Value == "F" )
		{
			curLPoint.Draw = true;
			glm::vec3 nextPos = curPos + curDir*len;

			curLPoint.Position = curPos;
			points.push_back(curLPoint);

			curLPoint.Position = nextPos;
			points.push_back(curLPoint);

			curPos = nextPos;
		}
		else if( mRHS[i]->Value == "f" )
		{
			curLPoint.Draw = false;
			glm::vec3 nextPos = curPos + curDir*len;

			curLPoint.Position = curPos;
			points.push_back(curLPoint);

			curLPoint.Position = nextPos;
			points.push_back(curLPoint);

			curPos = nextPos;
		}
		else if( mRHS[i]->Value == "+" )
		{
			glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(rotX), glm::vec3(1, 0, 0));
			glm::vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;
		}
		else if( mRHS[i]->Value == "-" )
		{
            glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(-rotX), glm::vec3(1, 0, 0));
			glm::vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;
		}
		else if( mRHS[i]->Value == "&" )
		{
            glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(rotY), glm::vec3(0, 1, 0));
			glm::vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;		
		}
		else if( mRHS[i]->Value == "^" )
		{
            glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(-rotY), glm::vec3(0, 1, 0));
			glm::vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;			
		}
		else if( mRHS[i]->Value == "\\" )
		{
            glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(rotZ), glm::vec3(0, 0, 1));
			glm::vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;				
		}
		else if( mRHS[i]->Value == "/" )
		{
            glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(-rotZ), glm::vec3(0, 0, 1));
			glm::vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;				
		}
		else if( mRHS[i]->Value == "|" )
		{
			curDir = -curDir;
		}
		else if( mRHS[i]->Value == "[" )
		{
			TurtleState state;
			state.Position = curPos;
			state.Direction = curDir;
			stateStack.push_back(state);
		}
		else if( mRHS[i]->Value == "]" )
		{
			TurtleState state = stateStack.back();
			stateStack.pop_back();

			curPos = state.Position;
			curDir = state.Direction;
		}
		else
		{
			RTGI_ASSERT( false );
		}
	}
}
//----------------------------------------------------------------------------