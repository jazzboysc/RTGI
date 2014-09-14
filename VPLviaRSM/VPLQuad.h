#ifndef RTGI_VPLQuad_H
#define RTGI_VPLQuad_H

#include "TriangleMesh.h"
#include "Texture1D.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/13/2014
//----------------------------------------------------------------------------
class VPLQuad : public TriangleMesh
{
public:
    VPLQuad(Material* material, Camera* camera);
    virtual ~VPLQuad();

	// Implement base class interface.
	virtual void OnGetShaderConstants();
	virtual void OnUpdateShaderConstants(int technique, int pass);

    Texture2DPtr RSMPositionTexture;
    Texture2DPtr RSMNormalTexture;
    Texture2DPtr RSMFluxTexture;
    Texture1DPtr RSMSampleTexture;

private:
    // pass 1 uniform locations.
    GLint mRSMPositionTextureLoc;
    GLint mRSMNormalTextureLoc;
    GLint mRSMFluxTextureLoc;
    GLint mRSMSampleTextureLoc;
};

typedef RefPointer<VPLQuad> VPLQuadPtr;

}

#endif