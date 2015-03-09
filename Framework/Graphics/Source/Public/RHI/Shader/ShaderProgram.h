//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ShaderProgram_H
#define RTGI_ShaderProgram_H

#include "RefObject.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "GeometryShader.h"
#include "ComputeShader.h"
#include "TessellationControlShader.h"
#include "TessellationEvaluationShader.h"
#include "ShaderUniform.h"

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

enum ShaderProgramParameter
{
    SPP_Geometry_Vertices_Out = 0,
    SPP_Max
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

	ShaderProgramInfo& operator<<(const char* _shaderName)
	{
		std::string shaderName = _shaderName;
		std::transform(shaderName.begin(), shaderName.end(), shaderName.begin(),
			::tolower);
		auto size = shaderName.size();
		auto fileExt = std::string(&shaderName.c_str()[size - 4]);
		if (fileExt == "vert")
		{
			VShaderFileName = shaderName;
			ShaderStageFlag |= ShaderType::ST_Vertex;
		}
		else if (fileExt == "frag")
		{
			FShaderFileName = shaderName;
			ShaderStageFlag |= ShaderType::ST_Fragment;
		}
		else if (fileExt == "geom")
		{
			FShaderFileName = shaderName;
			ShaderStageFlag |= ShaderType::ST_Geometry;
		}
		else if (fileExt == "ctrl")
		{
			TCShaderFileName = shaderName;
			ShaderStageFlag |= ShaderType::ST_TessellationControl;
		}
		else if (fileExt == "eval")
		{
			TEShaderFileName = shaderName;
			ShaderStageFlag |= ShaderType::ST_TessellationEvaluation;
		}
		return *this;
	}

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

    void GetUniformLocation(ShaderUniform* dstUniform, const char* name);

	ShaderProgramHandle* GetProgramHandle() const;
	VertexShader* GetVertexShader() const;
    FragmentShader* GetFragmentShader() const;
    GeometryShader* GetGeometryShader() const;
    ComputeShader* GetComputeShader() const;
    TessellationControlShader* GetTessellationControlShader() const;
    TessellationEvaluationShader* GetTessellationEvaluationShader() const;

    bool IsTessellationEnabled() const;
    bool IsComputeEnabled() const;
    bool IsVertexEnabled() const;

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