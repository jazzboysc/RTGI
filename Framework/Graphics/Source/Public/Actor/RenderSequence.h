//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RenderSequence_H
#define RTGI_RenderSequence_H

#include "RenderObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/30/2015
//----------------------------------------------------------------------------
class RenderSequence : public RenderObject
{
public:
    RenderSequence(Material* material, Camera* camera);
    virtual ~RenderSequence();

    // Implement base class interface.
    virtual int GetVoxelizerRasterDimension(Voxelizer* voxelizer);
    virtual Primitive* GetPrimitive() const;
    virtual Material* GetMaterial() const;
    virtual void SetRenderCache(RenderCache* renderCache);
    virtual RenderCache* GetRenderCache();
    virtual void UpdateRenderCache();
    virtual void Render(int technique, int pass,
        SubRenderer* subRenderer = 0);
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnRender(Pass* pass, PassInfo* passInfo);
    virtual void OnEnableBuffers();
    virtual void OnDisableBuffers();

    void AddRenderObject(RenderObject* renderObject);

protected:
    std::vector<RenderObjectPtr> mObjects;
    unsigned int mActiveObjectIndex;
};

typedef RefPointer<RenderSequence> RenderSequencePtr;

}

#endif