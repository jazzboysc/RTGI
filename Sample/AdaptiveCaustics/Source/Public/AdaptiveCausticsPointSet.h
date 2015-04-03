#pragma once

#include "PointSet.h"

namespace RTGI
{

	//----------------------------------------------------------------------------
	// Author: Che Sun
	// Date: 04/23/2014
	//----------------------------------------------------------------------------
	class AdaptiveCausticsPointSet : public PointSet
	{
	public:
		AdaptiveCausticsPointSet(Material* material);
		virtual ~AdaptiveCausticsPointSet();
		virtual void OnRender(Pass* pass, PassInfo* passInfo);

		// Implement base class interface.
		virtual void OnGetShaderConstants();
		virtual void OnUpdateShaderConstants(int technique, int pass);

		Texture2DArrayPtr NormalTextures;
		float deltaHierRes;
		float currHierarchyLevel;

		GLuint inputBuffer;
		GLuint outputBuffer;
	private:
		int posLoc;
		ShaderUniform mHierarchyLevelLoc;
		ShaderUniform mDeltaHierResLoc;
		ShaderUniform mSampler2DArrayLoc;
	};

	typedef RefPointer<AdaptiveCausticsPointSet> CausticsPointSetPtr;

}
