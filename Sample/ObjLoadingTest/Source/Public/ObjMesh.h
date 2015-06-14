#ifndef RTGI_ObjMesh_H
#define RTGI_ObjMesh_H

#include "TriangleMesh.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/14/2015
//----------------------------------------------------------------------------
class ObjMesh : public TriangleMesh
{
public:
    ObjMesh(Material* material, Camera* camera);
    virtual ~ObjMesh();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnUpdateShaderConstants(int technique, int pass);
};

typedef RefPointer<ObjMesh> ObjMeshPtr;

}

#endif