//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ShaderProgram_H
#define RTGI_ShaderProgram_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "GeometryShader.h"
#include "ComputeShader.h"
#include "TessellationControlShader.h"
#include "TessellationEvaluationShader.h"

namespace RTGI
{

enum ShaderType
{
    ST_Unknown                = 0,
    ST_Vertex                 = 1,
    ST_Fragment               = 2,
    ST_Geometry               = 4,
    ST_Compute                = 8,
    ST_TessellationControl    = 16,
    ST_TessellationEvaluation = 32
};

struct ShaderProgramInfo
{
    std::string VShaderFileName;
    std::string FShaderFileName;
    std::string GShaderFileName;
    std::string CShaderFileName;
    std::string TCShaderFileName;
    std::string TEShaderFileName;
    unsigned char ShaderStageFlag;

    ShaderProgramInfo(){ ShaderStageFlag = ShaderType::ST_Unknown; }
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class ShaderProgram : public RefObject
{
public:
    ShaderProgram(const ShaderProgramInfo& programInfo);
	ShaderProgram(VertexShader* vShader, FragmentShader* fShader);
	virtual ~ShaderProgram();

	void CreateDeviceResource(GPUDevice* device);

	ShaderProgramHandle* GetProgramHandle() const;
	VertexShader* GetVertexShader() const;
    FragmentShader* GetFragmentShader() const;
    GeometryShader* GetGeometryShader() const;
    ComputeShader* GetComputeShader() const;
    TessellationControlShader* GetTessellationControlShader() const;
    TessellationEvaluationShader* GetTessellationEvaluationShader() const;

    bool IsTessellationEnabled() const;
    bool IsComputeEnabled() const;

	void Enable();
	void Disable();

protected:
    ShaderProgramInfo mProgramInfo;

	VertexShaderPtr mVertexShader;
	FragmentShaderPtr mFragmentShader;
    GeometryShaderPtr mGeometryShader;
    ComputeShaderPtr mComputeShader;
    TessellationControlShaderPtr mTessellationControlShader;
    TessellationEvaluationShaderPtr mTessellationEvaluationShader;

	ShaderProgramHandle* mProgramHandle;
};

typedef RefPointer<ShaderProgram> ShaderProgramPtr;

}

#endif