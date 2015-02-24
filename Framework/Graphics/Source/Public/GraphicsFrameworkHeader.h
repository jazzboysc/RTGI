//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GraphicsFrameworkHeader_H
#define RTGI_GraphicsFrameworkHeader_H

#include "Application.h"

// Utils
#include "RefObject.h"
#include "RefPointer.h"
#include "Timer.h"
#include "Thread.h"

// Textures
#include "bmpread.h"
#include "Texture.h"
#include "Texture1D.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "TextureCube.h"
#include "Texture2DArray.h"
#include "FrameBuffer.h"
#include "TextureSampler.h"

// Buffers
#include "Buffer.h"
#include "IndexableBuffer.h"
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
#include "ShaderUniform.h"

// GPGPU
#include "ComputePass.h"
#include "ComputeTask.h"

// Scene Objects
#include "Camera.h"
#include "Fog.h"
#include "Light.h"
#include "PointSet.h"
#include "PolylineGeometry.h"
#include "RenderObject.h"
#include "TriangleMesh.h"
#include "ScreenQuad.h"
#include "GeometryAttributes.h"
#include "RenderSet.h"
#include "QuadMesh.h"

// Renderers
#include "BufferBase.h"
#include "RendererInput.h"
#include "RendererOutput.h"
#include "SubRenderer.h"
#include "GBufferRenderer.h"
#include "ShadowMapRenderer.h"
#include "Voxelizer.h"
#include "PipelineStateBlock.h"
#include "GPUDevice.h"
#include "GPUResource.h"

// OpenGL Device
#include "OpenGLDevice.h"
#include "OpenGLResource.h"

// L-System
#include "Expression.h"
#include "LSystem.h"
#include "PolyCylinder.h"
#include "Token.h"

// Utils
#include "GPUTimer.h"
#include "Terminal.h"

#endif