//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Token_H
#define RTGI_Token_H



namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/18/2013
//----------------------------------------------------------------------------
class Token
{
public:
	Token();
	~Token();

	enum TokenType
	{
		TT_Start,
		TT_Variable,
		TT_Operator,
		TT_Unknown
	};

	std::string Value;
	TokenType Type;
};

}

#endif