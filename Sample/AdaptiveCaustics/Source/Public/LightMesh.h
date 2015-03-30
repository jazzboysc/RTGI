#ifndef RTGI_LightMesh_H
#define RTGI_LightMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/22/2015
//----------------------------------------------------------------------------
class LightMesh : public TriangleMesh
{
public:
    LightMesh(Material* material, Camera* camera);
    virtual ~LightMesh();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnRender(Pass* pass, PassInfo* passInfo);

    Texture2DPtr LightMeshTexture;

private:
    ShaderUniform mLightMeshTextureSamplerLoc;
};

typedef RefPointer<LightMesh> LightMeshPtr;

}

#endif