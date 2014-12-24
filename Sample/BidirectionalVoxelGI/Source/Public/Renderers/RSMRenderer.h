#ifndef RTGI_RSMRenderer_H
#define RTGI_RSMRenderer_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

#define RTGI_RSMRenderer_RSMPosition_Name "RSMPosition"
#define RTGI_RSMRenderer_RSMNormal_Name "RSMNormal"
#define RTGI_RSMRenderer_RSMFlux_Name "RSMFlux"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class RSMRenderer : public SubRenderer
{
public:
    RSMRenderer(RenderSet* renderSet = 0);
    virtual ~RSMRenderer();

    void CreateRSM(int width, int height, int depth, 
        Texture::TextureFormat format);
    void Render(int technique, int pass, Camera* camera);

protected:
    PipelineStateBlockPtr mPSB;
};

typedef RefPointer<RSMRenderer> RSMRendererPtr;

}

#endif