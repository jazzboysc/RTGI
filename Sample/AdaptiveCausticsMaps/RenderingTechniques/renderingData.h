/***************************************************************************/
/* renderingData.h                                                         */
/* -----------------------                                                 */
/*                                                                         */
/* Stores a "class" that encapsulates global data used by the various      */
/*     rendering techniques described in this directory.  The internals of */
/*     this class are initialized by calling InitializeRenderingData()     */
/*     defined in initializeRenderingData.cpp.                             */
/*                                                                         */
/* Chris Wyman (02/23/2008)                                                */
/***************************************************************************/

#ifndef __RENDERING_DATA_H__
#define __REDNERING_DATA_H__

class FrameBuffer;
class GLSLProgram;
class GLTexture;
class FrameBufferData;
class VertexBufferData;
class TextureData;
class ShaderData;
class ParameterData;
class OtherGLIDData;
class UIData;
class MovieMaker;

// Class encapsulating global rendering data
class RenderingData
{
public:
	FrameBufferData  *fbo;
	ShaderData       *shader;
	ParameterData    *param;
	TextureData      *tex;
	VertexBufferData *vbo;
	OtherGLIDData    *glID;
	UIData           *ui;
};



class FrameBufferData
{
public:
	// Framebuffers for simple rendering with shadow maps
	FrameBuffer *shadowMap[4], *mainWin, *helpScreen;

	// Framebuffers for refraction rendering (some reused by caustics)
	FrameBuffer *refract_NormalMap, *refract_BackgroundOBJs;

	// Framebuffers for simple caustic map rendering
	FrameBuffer *caustic_PhotonBuffer, *caustic_BackgroundOBJs;
	FrameBuffer *causticMap[4], *causticMapSmall;

	// Framebuffers for hierarchical caustic map rendering
	FrameBuffer *causticMapLayered;

	// Framebuffers used for deferred rendering 
	FrameBuffer *deferredCaustic_refractorGeom, *deferredRefract_refractorGeom;
};


class ShaderData
{
public:
	// Utility shaders
	GLSLProgram *copyColorAndDepth;
	GLSLProgram *combineMipmapArrayLevels;
	GLSLProgram *outputEyeSpacePosition;
	GLSLProgram *copyDepth;

	// Shaders for refraction rendering (see Wyman, SIGGRAPH 2005)
	GLSLProgram *refract_DrawNormals, *refract;

	// Shaders for simple caustic map rendering (see Wyman & Davis, I3D 2006)
	GLSLProgram *caustic_DrawPhotonBuf, *caustic_SimplePhotonSplat;

	// Shaders for hierarchical caustic map (HCM) rendering (see Wyman, I3D 2008)
	GLSLProgram *caustic_TraversePhotonHier, *caustic_SplatPhotonHier;

	// Shaders for deferred refraction (see this paper, Wyman & Nichols, EG 2009)
	GLSLProgram *deferredRefraction, *deferRefract_renderFrontAndBackNorms;

	// Shaders for adaptive traversal inside the adaptiveCaustics.cpp render mode
	//    (i.e., adaptive traversal, image-space splatting (no HCM), max traversal metric)
	GLSLProgram *adaptCaustic_traverse, *adaptCaustic_splat;

	// Shaders for adaptive traversal inside the adaptiveCausticsErrorMetricWithBatching.cpp 
	//    render mode.  (i.e., adaptive traversal, simplistic HCM output, max error metric)
	// Note:  This render mode also reuses GLSLProgram *adaptCaustic_traverse; from above.
	GLSLProgram *adaptiveCaustics_errorMetric_Traversal;
	GLSLProgram *adaptiveCaustics_errorMetric_detectConvergence;
	GLSLProgram *adaptiveCaustics_errorMetric_SplatPhotons;
	GLSLProgram *adaptiveCaustics_errorMetric_streamOutConverged;
	GLSLProgram *adaptiveCaustics_errorMetric_streamOutUnconverged;
};


class ParameterData
{
public:
	// Parameters for simple rendering with shadow maps
	float shadowMatrixTrans[4][16];
	//bool *usingShadows;
	float *shadowMapBias, *lightFOV;

	// Parameters for refraction
	float *indexOfRefraction;
	float refract_ShaderLocal2[4];

	// A simple matrix representing gluOrtho2D(0,1,0,1);
	float gluOrtho[16];

	// A user-changable parameter specifying the maximum traversal level.
	int *maxTraversalLevel;
};

class TextureData
{
public:
	// Spotlight texture.  This is typically needed only in the Scene class,
	//    but sometimes (e.g., with caustics) it may be needed elsewhere.
	GLTexture *spotlight, *lowResSpot, *blurrySpot;
	GLTexture *iowaLogo;
};

class VertexBufferData
{
public:
	// Vertex buffers used when photons, photon buffers, or caustic splats.  
	//   Multiple buffers are used for various types of ping-ponging computation.
	GLuint genericTraversalStartBuffer;
	GLuint genericTraversalBuffer[5];
};

class OtherGLIDData
{
public:
	// Occlusion Query IDs
	GLuint primCountQuery;
};

class UIData
{
public:
	bool updateHelpScreen;
	bool displayHelp;
	bool captureScreen;
	bool *animation;
	int *adaptiveDepthParam;
	int *numBatches, *numLightsUsed;
};



#endif

