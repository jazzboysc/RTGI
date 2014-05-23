//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Expression.h"

using namespace RTGI;

struct TurtleState
{
	vec3 Position;
	vec3 Direction;
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
	vec3& initPos, vec3& initDir, float len, float rotX, float rotY, 
	float rotZ) const
{
	vec3 curPos = initPos;
	vec3 curDir = initDir;
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
			vec3 nextPos = curPos + curDir*len;

			curLPoint.Position = curPos;
			points.push_back(curLPoint);

			curLPoint.Position = nextPos;
			points.push_back(curLPoint);

			curPos = nextPos;
		}
		else if( mRHS[i]->Value == "f" )
		{
			curLPoint.Draw = false;
			vec3 nextPos = curPos + curDir*len;

			curLPoint.Position = curPos;
			points.push_back(curLPoint);

			curLPoint.Position = nextPos;
			points.push_back(curLPoint);

			curPos = nextPos;
		}
		else if( mRHS[i]->Value == "+" )
		{
			mat4 rot = RotateX(rotX);
			vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;
		}
		else if( mRHS[i]->Value == "-" )
		{
			mat4 rot = RotateX(-rotX);
			vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;
		}
		else if( mRHS[i]->Value == "&" )
		{
			mat4 rot = RotateY(rotY);
			vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;		
		}
		else if( mRHS[i]->Value == "^" )
		{
			mat4 rot = RotateY(-rotY);
			vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;			
		}
		else if( mRHS[i]->Value == "\\" )
		{
			mat4 rot = RotateZ(rotZ);
			vec4 curDirH(curDir, 0.0f);
			curDirH = rot * curDirH;
			curDir.x = curDirH.x;
			curDir.y = curDirH.y;
			curDir.z = curDirH.z;				
		}
		else if( mRHS[i]->Value == "/" )
		{
			mat4 rot = RotateZ(-rotZ);
			vec4 curDirH(curDir, 0.0f);
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
			assert( false );
		}
	}
}
//----------------------------------------------------------------------------