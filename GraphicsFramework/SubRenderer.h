//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_SubRenderer_H
#define RTGI_SubRenderer_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Texture.h"
#include "RendererOutput.h"
#include "SceneManager.h"
#include <vector>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class SubRenderer : public RefObject
{
public:
    SubRenderer(SceneManager* sceneManager);
    virtual ~SubRenderer();

    void AddRenderTarget(const std::string& name, int width, int height,
        Texture::TextureFormat format);
    int GetRenderTargetCount() const;
    RendererOutput* GetRenderTarget(int i) const;
    RendererOutput* GetRenderTargetByName(const std::string& name) const;
    RendererOutput* GetDepthTarget() const;

    void SetInputDependency(SubRenderer* producer, const std::string& name, 
        int slot);

    void CreateFrameBuffer(int depthWidth, int depthHeight);

    void Render(int technique, int pass);

    enum { MAX_INPUT_DEPENDENCY_COUNT = 16 };

protected:
    RendererOutputPtr mInputs[MAX_INPUT_DEPENDENCY_COUNT];
    std::vector<RendererOutputPtr> mRenderTargets;
    RendererOutputPtr mDepthTarget;
    FrameBufferPtr mFrameBuffer;

    SceneManagerPtr mSceneManager;
};

typedef RefPointer<SubRenderer> SubRendererPtr;

}

#endif