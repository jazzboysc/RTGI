//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GBufferRenderer_H
#define RTGI_GBufferRenderer_H

#include "SubRenderer.h"

namespace RTGI
{

#define RTGI_GBuffer_Position_Name "Position"
#define RTGI_GBuffer_Normal_Name   "Normal"
#define RTGI_GBuffer_Albedo_Name   "Albedo"

struct GBufferDesc
{
    int Width;
    int Height;
    BufferFormat PositionFormat;
    BufferFormat NormalFormat;
    BufferFormat AlbedoFormat;
    bool PositionMipmap;
    bool NormalMipmap;
    bool AlbedoMipmap;

    GBufferDesc()
    {
        Width = 0;
        Height = 0;
        PositionFormat = BF_Unknown;
        NormalFormat = BF_Unknown;
        AlbedoFormat = BF_Unknown;
        PositionMipmap = false;
        NormalMipmap = false;
        AlbedoMipmap = false;
    }
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class GBufferRenderer : public SubRenderer
{
public:
    GBufferRenderer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~GBufferRenderer();

    void CreateGBuffer(GBufferDesc* desc);
    void Render(int technique, int pass, Camera* camera);

protected:
    PipelineStateBlockPtr mPSB;
};

typedef RefPointer<GBufferRenderer> GBufferRendererPtr;

}

#endif