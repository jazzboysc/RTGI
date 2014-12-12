//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_SubRenderer_H
#define RTGI_SubRenderer_H


#include "RefObject.h"
#include "Texture.h"
#include "RendererInput.h"
#include "RendererOutput.h"
#include "RenderSet.h"
#include "Camera.h"
#include "PipelineStateBlock.h"
#include "GPUTimer.h"
#include <vector>

namespace RTGI
{

enum SubRendererOutput
{
    SRO_Unknown       = 0,
    SRO_FrameBuffer   = 1,
    SRO_GenericBuffer = 2
};

typedef BufferBase* (*SubRendererCreateRendererData)(int size, 
    BufferUsage usage);

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class SubRenderer : public RefObject
{
public:
    SubRenderer(RenderSet* renderSet = 0);
    virtual ~SubRenderer();

    // Scene rendered by this renderer.
    void SetRenderSet(RenderSet* renderSet);
    RenderSet* GetRenderSet() const;

    // Renderer Inputs stuff.
    enum { MAX_INPUT_DEPENDENCY_COUNT = 16 };
    void AddInputDependency(SubRenderer* producer, const std::string& srcName,
        RendererInputDataView* view);
    RendererInput* GetInputDependency(int i) const;
    void ClearInputDependency();

    // FrameBuffer stuff.
    void AddFrameBufferTarget(const std::string& name, int width, int height, 
        int depth, Texture::TextureType type, Texture::TextureFormat format);
    int GetFrameBufferTargetCount() const;
    RendererOutput* GetFrameBufferTarget(int i) const;
    RendererOutput* GetFrameBufferTargetByName(const std::string& name) const;
    RendererOutput* GetDepthTarget() const;
    Texture* GetFrameBufferTexture(int i) const;
    Texture* GetFrameBufferTextureByName(const std::string& name) const;
    Texture* GetDepthTexture() const;

    void CreateFrameBuffer(int depthWidth, int depthHeight, int depthCount,
        Texture::TextureType depthType);

    // GenericBuffer stuff.
    void AddGenericBufferTarget(const std::string& name, 
        RendererDataType bufferType, int size, BufferUsage usage, 
        BindingFlag flag, unsigned int binding);
    int GetGenericBufferTargetCount() const;
    RendererOutput* GetGenericBufferTarget(int i) const;
    RendererOutput* GetGenericBufferTargetByName(const std::string& name) const;

    // Rendering stuff.
    virtual void Render(int technique, int pass, SubRendererOutput outputFlag, 
        PipelineStateBlock* psb, Camera* camera = 0);
    virtual void RenderSingle(RenderObject* object, int technique, int pass, 
        SubRendererOutput outputFlag, PipelineStateBlock* psb, 
        Camera* camera = 0);
    virtual void OnRender(int technique, int pass, Camera* camera);

    void SetTimer(GPUTimer* timer);
    GPUTimer* GetTimer() const;
    double GetTimeElapsed() const;

protected:
    void PreRender(SubRendererOutput outputFlag, PipelineStateBlock* psb);
    void PostRender(SubRendererOutput outputFlag, PipelineStateBlock* psb);
    void ApplyPipelineStateBlock(PipelineStateBlock* psb);

    // Buffer inputs.
    std::vector<RendererInputPtr> mInputs;

    // Buffer outputs.
    std::vector<RendererOutputPtr> mFrameBufferTargets;
    RendererOutputPtr mDepthTarget;
    FrameBufferPtr mFrameBuffer;
    std::vector<RendererOutputPtr> mGenericBufferTargets;

    // Scene Inputs.
    RenderSetPtr mRenderSet;

    GPUTimerPtr mTimer;

private:
    static SubRendererCreateRendererData msFactoryFunctions[6];
};

typedef RefPointer<SubRenderer> SubRendererPtr;

}

#endif