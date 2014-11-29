//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_SubRenderer_H
#define RTGI_SubRenderer_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Texture.h"
#include "RenderTargetTexture.h"
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
    SubRenderer();
    virtual ~SubRenderer();

    void AddRenderTarget(const std::string& name, int width, int height,
        Texture::TextureFormat format);
    int GetRenderTargetCount() const;
    RenderTargetTexture* GetRenderTarget(int i) const;
    RenderTargetTexture* GetRenderTargetByName(const std::string& name) const;
    RenderTargetTexture* GetDepthTarget() const;

    void CreateFrameBuffer(int depthWidth, int depthHeight);

protected:
    std::vector<RenderTargetTexturePtr> mRenderTargets;
    RenderTargetTexturePtr mDepthTarget;
    FrameBufferPtr mFrameBuffer;
};

typedef RefPointer<SubRenderer> SubRendererPtr;

}

#endif