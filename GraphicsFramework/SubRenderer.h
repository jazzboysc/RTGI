//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_SubRenderer_H
#define RTGI_SubRenderer_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Texture.h"
#include "RendererInput.h"
#include "RendererOutput.h"
#include "SceneManager.h"
#include <vector>

namespace RTGI
{

enum SubRendererOutput
{
    SRO_Unknown       = 0,
    SRO_FrameBuffer   = 1,
    SRO_GenericBuffer = 2
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class SubRenderer : public RefObject
{
public:
    SubRenderer(SceneManager* sceneManager);
    virtual ~SubRenderer();

    enum { MAX_INPUT_DEPENDENCY_COUNT = 16 };
    void AddInputDependency(SubRenderer* producer, const std::string& srcName,
        RendererDataView* view);

    // FrameBuffer stuff.
    void AddFrameBufferTarget(const std::string& name, int width, int height,
        Texture::TextureFormat format);
    int GetFrameBufferTargetCount() const;
    RendererOutput* GetFrameBufferTarget(int i) const;
    RendererOutput* GetFrameBufferTargetByName(const std::string& name) const;
    RendererOutput* GetDepthTarget() const;
    void CreateFrameBuffer(int depthWidth, int depthHeight);

    // GenericBuffer stuff.
    void AddGenericBufferTarget(const std::string& name, 
        RendererDataType bufferType, int size, BufferUsage usage);
    int GetGenericBufferTargetCount() const;
    RendererOutput* GetGenericBufferTarget(int i) const;
    RendererOutput* GetGenericBufferTargetByName(const std::string& name) const;

    // Rendering stuff.
    virtual void Render(int technique, int pass, SubRendererOutput outputFlag);


protected:
    // Inputs.
    std::vector<RendererInputPtr> mInputs;

    // Outputs.
    std::vector<RendererOutputPtr> mFrameBufferTargets;
    RendererOutputPtr mDepthTarget;
    FrameBufferPtr mFrameBuffer;
    std::vector<RendererOutputPtr> mGenericBufferTargets;

    // Scene Inputs.
    SceneManagerPtr mSceneManager;
};

typedef RefPointer<SubRenderer> SubRendererPtr;

}

#endif