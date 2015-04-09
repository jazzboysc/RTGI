#include "AdaptiveCausticsPointSet.h"
#include "AdaptiveCausticsApp.h"

using namespace RTGI;

//                 |
//                 | vertices in
//                 V
//               vertex
//               shader
//                 |
//                 V
//              primitive
//              assembly
//                 |
//                 +-----------+
//                 |           |
//                 |      tessellation
//                 |           |
//                 +<----------+
//                 |        
//                 +---------------+
//                 |               |
//                 |stream         V
//                 |0           geometry
//                 |             shader
//                 |            | | | |
//                 |            V V V V
//                 |            primitive
//                 |            assembly
//                 V            | | | |
//                 +------------+ | | |
//                 |  stream 0    | | |
//                 |              | | |
//                 |  +-----------+ | |
//                 |  |stream 1     | |
//                 |  |             | |
//                 |  |     stream 2| |stream 3
//                 |  |  +----------+ |
//                 |  |  |  +---------+
//                 |  |  |  |
//                 V  V  V  V
//             transform feedback ----+--------> binding point 0 ----> buffer
//                 |                  +--------> binding point 1 ----> buffer
//                 | stream 0         +--------> binding point 2 ----> buffer
//                 +---+              +--------> binding point 3 ----> buffer
//                     |
//                     V
//                 clipping,
//                rasterization,
//                   etc...

//----------------------------------------------------------------------------
AdaptiveCausticsPointSet::AdaptiveCausticsPointSet(Material* material)
:
PointSet(material)
{}
//----------------------------------------------------------------------------
AdaptiveCausticsPointSet::~AdaptiveCausticsPointSet()
{}
//----------------------------------------------------------------------------
void AdaptiveCausticsPointSet::OnRender(Pass* pass, PassInfo* passInfo)
{
	auto program = this->mMaterial->GetProgram(0, 0);
	auto HProgram = program->GetProgramHandle();
	// Setup transform feedback to allow readback of the positions output
	const GLchar* Varyings[1];
	Varyings[0] = "gl_Position";
	GLuint prog = *(((GLuint*)HProgram + 3));
	//int posLoc = glGetVaryingLocationNV((GLuint)HProgram, "gl_Position");
	glTransformFeedbackVaryings((GLuint)prog, 1, Varyings, GL_INTERLEAVED_ATTRIBS);
}
//----------------------------------------------------------------------------
void AdaptiveCausticsPointSet::OnGetShaderConstants()
{
	auto program = mMaterial->GetProgram(0, 0);
	auto HProgram = program->GetProgramHandle();

	const GLchar* Varyings[1];
	Varyings[0] = "gl_Position";
	//int posLoc = glGetVaryingLocationNV((GLuint)HProgram, "gl_Position");
	GLuint prog = *(((GLuint*)HProgram + 3));
	glTransformFeedbackVaryings(prog, 1, Varyings, GL_INTERLEAVED_ATTRIBS);

	GLuint a;
	glGenBuffers(1, &a);
#ifdef _DEBUG
	GLenum r1es = glGetError();
	RTGI_ASSERT(r1es == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void AdaptiveCausticsPointSet::OnUpdateShaderConstants(int technique, int pass)
{
	glm::mat4 worldTrans = mSpatialInfo->GetWorldTransform();

	switch (pass)
	{
	default:
		break;
	case 0:
		mHierarchyLevelLoc.SetValue(worldTrans);
		mDeltaHierResLoc.SetValue(worldTrans);
		mSampler2DArrayLoc.SetValue(worldTrans);
		break;
	}

	SamplerDesc sampler;
	sampler.MinFilter = FT_Nearest;
	sampler.MagFilter = FT_Nearest;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;

	mSampler2DArrayLoc.SetValue(0);
	NormalTextures->BindToSampler(0, &sampler);
}
//----------------------------------------------------------------------------