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
    RenderSequence(Material* material);
    virtual ~RenderSequence();

    // Implement base class interface.
    virtual void SetCamera(Camera* camera);
    virtual Camera* GetCamera() const;
    virtual int GetVoxelizerRasterDimension(Voxelizer* voxelizer);
    virtual Primitive* GetPrimitive() const;
    virtual Material* GetMaterial() const;
    virtual void SetWorldTransform(const glm::mat4& worldTrans);
    virtual glm::mat4 GetWorldTransform() const;
    virtual void SetWorldTranslation(const glm::vec3& translation);
    virtual glm::vec3 GetWorldTranslation() const;
    virtual void SetWorldScale(const glm::vec3& scale);
    virtual glm::vec3 GetWorldScale() const;
    virtual void SetRenderCache(RenderCache* renderCache);
    virtual RenderCache* GetRenderCache();
    virtual void UpdateRenderCache();
    virtual void Render(int technique, int pass,
        SubRenderer* subRenderer = 0);
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnRender(Pass* pass, PassInfo* passInfo);
    virtual void OnEnableBuffers();
    virtual void OnDisableBuffers();

    void Update(unsigned int frame);
    void SetActiveRenderObject(unsigned int index);
    void SetFrequence(float value);
    void AddRenderObject(RenderObject* renderObject);

protected:
    std::vector<RenderObjectPtr> mObjects;
    unsigned int mActiveObjectIndex;
    float mFrequence;
};

typedef RefPointer<RenderSequence> RenderSequencePtr;

}

#endif