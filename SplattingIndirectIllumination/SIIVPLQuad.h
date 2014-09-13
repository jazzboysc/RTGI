#ifndef RTGI_SIIVPLQuad_H
#define RTGI_SIIVPLQuad_H

#include "TriangleMesh.h"
#include "Texture1D.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/13/2014
//----------------------------------------------------------------------------
class SIIVPLQuad : public TriangleMesh
{
public:
    SIIVPLQuad(Material* material, Camera* camera);
    virtual ~SIIVPLQuad();

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

typedef RefPointer<SIIVPLQuad> SIIVPLQuadPtr;

}

#endif