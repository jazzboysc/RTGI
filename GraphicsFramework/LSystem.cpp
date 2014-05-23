//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "LSystem.h"

using namespace RTGI;

//----------------------------------------------------------------------------
LSystem::LSystem()
{
	mLen = 0;
	mIter = 0;
	mRotX = 0.0f;
	mRotY = 0.0f;
	mRotZ = 0.0f;
}
//----------------------------------------------------------------------------
LSystem::~LSystem()
{
	delete mStart;
	for( int i = 0; i < (int)mVariables.size(); ++i )
	{
		delete mVariables[i];
	}
}
//----------------------------------------------------------------------------
bool LSystem::LoadFromFile(const std::string& fileName)
{
	std::ifstream fileData(fileName);
    if( !fileData )
    {
        return false;
    }

	std::string curLine;
	std::string curValue;
	std::string::size_type uiBegin, uiEnd;

	while( !fileData.eof() )
	{
		getline(fileData, curLine);
		if( curLine.size() > 0 && curLine[0] != '#' )
		{
			break;
		}
	}

	uiBegin = 0;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	assert( curValue == "len:" );
	uiBegin = curLine.find_first_not_of(" ", uiEnd);
	uiEnd = curLine.find("\n", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mLen = atoi(curValue.c_str());

	getline(fileData, curLine);
	uiBegin = 0;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	assert( curValue == "iter:" );
	uiBegin = curLine.find_first_not_of(" ", uiEnd);
	uiEnd = curLine.find("\n", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mIter = atoi(curValue.c_str());

	getline(fileData, curLine);
	uiBegin = 0;
	uiBegin = curLine.find_first_not_of(" ", uiBegin);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	assert( curValue == "rot:" );
	uiBegin = curLine.find_first_not_of(" ", uiEnd);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mRotX = (float)atof(curValue.c_str());
	uiBegin = curLine.find_first_not_of(" ", uiEnd);
	uiEnd = curLine.find(" ", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mRotY = (float)atof(curValue.c_str());
	uiBegin = curLine.find_first_not_of(" ", uiEnd);
	uiEnd = curLine.find("\n", uiBegin);
	curValue = curLine.substr(uiBegin, uiEnd - uiBegin);
	mRotZ = (float)atof(curValue.c_str());

	// Parse expressions.
	while( !fileData.eof() )
	{
		getline(fileData, curLine);
		if( curLine.size() > 0 && curLine[0] != '#' )
		{
			Expression* exp = ParseExpression(curLine);
			Token* t = exp->GetLHS();
			if( t->Type == Token::TT_Start )
			{
				mStart = exp;
			}
			else
			{
				mVariables.push_back(exp);
			}
		}
	}

	fileData.close();
	return true;
}
//----------------------------------------------------------------------------
Expression* LSystem::ParseExpression(const std::string& expression)
{
	Expression* exp = new Expression;

	std::string::size_type uiBegin, uiEnd;
	uiBegin = 0;
	uiBegin = expression.find_first_not_of(" ", uiBegin);
	uiEnd = expression.find(":", uiBegin);
	exp->mLHS->Value = expression.substr(uiBegin, uiEnd - uiBegin);
	if( exp->mLHS->Value == "start" )
	{
		exp->mLHS->Type = Token::TT_Start;
	}
	else
	{
		exp->mLHS->Type = Token::TT_Variable;
	}

	uiBegin = expression.find_first_not_of(" ", uiEnd + 1);
	uiEnd = expression.find("\n", uiBegin);
	std::string str = expression.substr(uiBegin, uiEnd - uiBegin);

	for( int i = 0; i < (int)str.length(); ++i )
	{
		Token* t = new Token;
		t->Value = str.substr(i, 1);
		if( t->Value[0] == '+'  || 
			t->Value[0] == '-'  ||
			t->Value[0] == '&'  ||
			t->Value[0] == '^'  ||
			t->Value[0] == '\\' || 
			t->Value[0] == '/'  ||
			t->Value[0] == '|'  ||
			t->Value[0] == '['  ||
			t->Value[0] == ']' )
		{
			t->Type = Token::TT_Operator;
		}
		else
		{
			t->Type = Token::TT_Variable;
		}

		exp->mRHS.push_back(t);
	}

	return exp;
}
//----------------------------------------------------------------------------
Expression* LSystem::FindVariableByName(const std::string& name)
{
	for( int i = 0; i < (int)mVariables.size(); ++i )
	{
		if( mVariables[i]->mLHS->Value == name )
		{
			return mVariables[i];
		}
	}

	return 0;
}
//----------------------------------------------------------------------------
Expression* LSystem::GetStartExpression()
{
	return mStart;
}
//----------------------------------------------------------------------------
Expression* LSystem::GenerateIteration(Expression* exp)
{
	Expression* res = new Expression;
	res->mLHS->Value = exp->mLHS->Value;
	res->mLHS->Type = exp->mLHS->Type;

	size_t tokenCount = exp->mRHS.size();
	for( unsigned int i = 0; i < tokenCount; ++i )
	{
		Token* t= exp->mRHS[i];
		if( t->Type == Token::TT_Operator )
		{
			Token* newOperator = new Token;
			newOperator->Type = Token::TT_Operator;
			newOperator->Value = t->Value;

			res->mRHS.push_back(newOperator);
		}
		else if( t->Type == Token::TT_Variable )
		{
			Expression* variable = FindVariableByName(t->Value);
			if( variable == 0 )
			{
				// Find nothing.
				Token* newVariable = new Token;
				newVariable->Type = Token::TT_Variable;
				newVariable->Value = t->Value;

				res->mRHS.push_back(newVariable);
			}
			else
			{
				unsigned int newCount = (unsigned int)variable->mRHS.size();
				for( unsigned int j = 0; j < newCount; ++j )
				{
					Token* newToken = new Token;
					newToken->Type = variable->mRHS[j]->Type;
					newToken->Value = variable->mRHS[j]->Value;

					res->mRHS.push_back(newToken);
				}
			}
		}
		else
		{
			assert( false );
		}
	}

	return res;
}
//----------------------------------------------------------------------------
void LSystem::GetParams(int& len, int& iter, float& rotX, float& rotY, 
	float& rotZ) const
{
	len = mLen;
	iter = mIter;
	rotX = mRotX;
	rotY = mRotY;
	rotZ = mRotZ;
}
//----------------------------------------------------------------------------