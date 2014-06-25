//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture2D_H
#define RTGI_Texture2D_H

#include "FrameworkCommon.h"
#include "Texture.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2013
//----------------------------------------------------------------------------
class Texture2D : public Texture
{
public:
	Texture2D();
	~Texture2D();

	enum RenderTargetFormat
	{
		RTF_RGB,
		RTF_RGBF,
		RTF_Depth
	};

	// Load a ".bmp" file and create its resource.
	bool LoadBMPFromFile(const std::string& fileName);

	// Load a ".pfm" file and create its resource.
	bool LoadPFMFromFile(const std::string& fileName);

	// Load texture data from system memory. User is responsible for deleting
	// the system memory data.
	bool LoadFromSystemMemory(GLint internalFormat, GLsizei width, 
		GLsizei height, GLenum format, GLenum type, void* pixels);

	void CreateRenderTarget(int width, int height, RenderTargetFormat format);

	// Generate random positions inside a unit hemisphere based on halton 
	// numbers.
	void CreateLDRandomTexture(int maxSampleCount, int patternSize);

	int Width, Height;
	bool IsRenderTarget;
	bool IsHDRTexture;
	float LMax;
	float RevGamma;
	RenderTargetFormat RTFormat;
};

typedef RefPointer<Texture2D> Texture2DPtr;

}

#endif