//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GraphicsFrameworkHeader_H
#define RTGI_GraphicsFrameworkHeader_H

#include "FrameworkCommon.h"
#include "Application.h"
#include "Angel.h"

// Textures
#include "bmpread.h"
#include "Texture.h"
#include "Texture1D.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "TextureCube.h"
#include "Texture2DArray.h"
#include "FrameBuffer.h"

// Buffers
#include "Buffer.h"
#include "PixelBuffer.h"
#include "AtomicCounterBuffer.h"
#include "TextureBuffer.h"
#include "StructuredBuffer.h"
#include "DispatchIndirectBuffer.h"
#include "UniformBuffer.h"

// Material
#include "Shader.h"
#include "VertexShader.h"
#include "GeometryShader.h"
#include "FragmentShader.h"
#include "ComputeShader.h"
#include "TessellationControlShader.h"
#include "TessellationEvaluationShader.h"
#include "ShaderProgram.h"
#include "PassBase.h"
#include "Pass.h"
#include "PassInfo.h"
#include "PassManager.h"
#include "Technique.h"
#include "TechniqueInfo.h"
#include "MaterialTemplate.h"

// GPGPU
#include "ComputePass.h"
#include "ComputeTask.h"

// Math
#include "vec.h"
#include "mat.h"
#include "AABB.h"
#include "RNG.h"

// Scene Objects
#include "Camera.h"
#include "Fog.h"
#include "Light.h"
#include "PointSet.h"
#include "PolylineGeometry.h"
#include "RefObject.h"
#include "RefPointer.h"
#include "RenderObject.h"
#include "TriangleMesh.h"
#include "ScreenQuad.h"
#include "GeometryAttributes.h"
#include "SceneManager.h"

// Renderers
#include "SubRenderer.h"
#include "RenderTargetTexture.h"

// L-System
#include "Expression.h"
#include "LSystem.h"
#include "PolyCylinder.h"
#include "Token.h"

// Utils
#include "GPUTimer.h"

#endif