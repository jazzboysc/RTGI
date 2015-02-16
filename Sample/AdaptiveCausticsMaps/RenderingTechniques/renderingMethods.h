/***************************************************************************/
/* renderingMethods.h                                                      */
/* -----------------------                                                 */
/*                                                                         */
/* Has prototypes and other data needed to use the various rendering       */
/*     techniques (i.e., DisplayCallback()'s) located in the .cpp files    */
/*     in this directory.                                                  */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/

#ifndef __REDNERINGMETHODS_H__
#define __REDNERINGMETHODS_H__

class FrameBuffer;

// Display routines.  Currently, these are called via a case statement
//   inside DisplayCallback() inside sceneLoader.cpp
void Display_WithRefraction( void );				         // See: basicRefraction.cpp
void Display_WithSimpleRefractiveCaustics( void );	         // See: simpleCausticMaps.cpp
void Display_WithHierarchicalRefractiveCaustics( void );     // See: hierarchicalCausticMaps.cpp
void Display_WithVolumetricShadows( void );                  // See: volumetricShadows.cpp

// Experimental
void Display_WithVolumetricShadows2( void );
void Display_UsingGPC( void );
void Display_WithVolumetricCaustics( void );				 // See: volumetricCaustics.cpp
void Display_WithVolumetricCaustics2( void );                // See: volumetricCaustics2.cpp

void Display_WithAdaptiveRefraction( void );
void Display_WithAdaptiveCaustics( void );
void Display_WithAdaptiveHierarchicalCaustics( void );
void Display_WithAdaptiveCaustics2( void );
void Display_WithExperimentalAdaptiveCaustics( void );
void Display_WithDeferredRefraction( void );
void Display_WithMultiLayeredDeferredRefraction( void );
void Display_WithSimpleDeferredCaustics( void );
void Display_WithOriginalRefractiveCaustics( void );
void Display_WithOriginalHierarchicalCaustics( void );
void Display_WithDeferredHierarchicalRefractiveCaustics( void );
void Display_WithAdaptiveHierarchicalRefractiveCaustics( void );
void Display_WithAdaptiveHierarchicalRefractiveCaustics2( void );
void Display_WithAdaptiveCausticsUsingErrorMetric( void );
void Display_WithMultiLayerAdaptiveCaustics( void );
void Display_WithDeferredMultiLayerHierarchicalRefractiveCaustics( void );
void Display_WithAdaptiveCausticsUsingErrorMetricAndBatching( void );

//void DeferredRefraction( void );
void DeferredRefraction( FrameBuffer *background, float *glassColor );
void CreateBuffersForDeferredRefraction( bool withCaustics, bool mipmapped );
void CreateNonRefractiveObjectTexture( bool useShadows, bool useCaustics=false );

// The data structure in "renderingData.h" stores various data needed
//    by the rendering techniques in this directory.  This function 
//    initializes all this data.
void InitializeRenderingData( void );					     // See: initializeRenderingData.cpp
void FreeMemoryAndQuit( void );


void DisplayParameters( void );

// Utility routines used by multiple rendering techniques
void BeginPrimitiveCount( GLenum primType, GLuint queryID );      // See: utilRoutines.cpp
int EndPrimitiveCount( GLuint queryID );                          // See: utilRoutines.cpp
float sqr( float f );                                             // See: utilRoutines.cpp
float log2( float x );										      // See: utilRoutines.cpp
void SetupAirlightModel_FTexture( GLuint airlightID );            // See: utilRoutines.cpp
void CreatePerFrameAirlightTexture( FrameBuffer *frame,           // See: utilRoutines.cpp
								    float *eyeSpaceLightPos, 
									float maxDist = 50.0 );
void DrawLightVolume( FrameBuffer *frame,                         // See: utilRoutines.cpp
					  unsigned int lightNum = 0 );
void CombineDepthTexturesIntoCurrentFBODepth( GLuint depthTex1,   // See: utilRoutines.cpp
											  GLuint depthTex2 );  
void CopyColorAndDepthTexturesIntoCurrentFBO( GLuint colorTex,    // See: utilRoutines.cpp
											  GLuint depthTex );  
void CopyDepthToCausticMap( FrameBuffer *cMap, FrameBuffer *inputDepth );


// Performs a hierarchical traversal using the specified traversal shader and a
//    transform-feedback loop to start at the lowest level, use output from the geometry
//    shader as input, and repeat down the hierarchy.  This is good for, for instance,
//    a breadth-first traversal of a quad-tree represented by a mipmap hierarchy, where
//    many sub-trees can be discarded at a high level so individual leaves need not be
//    touched en mass.  (In utilRoutines.cpp)
void HierarchicalTransformFeedbackTraversal( GLSLProgram *traverse, // Shader to traverse the hierarchy
											 GLSLProgram *render,   // Shader to perform the render at the final level
											 int startLevel,        // Level to start traversal from (root = 0)
											 int lightNum );        // Specifies the light we're currently creating
void HierarchicalTransformFeedbackTraversalWithBatching( GLSLProgram *traverse,
											             GLSLProgram *render,
											             int startLevel,
											             int lightNum );

// Code useful for caustics, but used among multiple renderers that generate caustics.
void SetLightMatrices( int lightNum, float aspect );              // See: simpleCausticMaps.cpp
void PopLightMatrices( void );						              // See: simpleCausticMaps.cpp
void CreateBackgroundTextureWithCaustics( FrameBuffer *fbo );     // See: simpleCausticMaps.cpp
void CreateRefractiveNormalMaps( int );			                  // See: basicRefraction.cpp
void CombineMipmapArrayLevels( GLuint arrayTex );                 // See: hierarchicalCausticMaps.cpp


void SetCurrentVertexBuffer( GLuint vboID, GLuint offset = 0 );
void BeginTimerQuery( GLuint queryID );
void EndTimerQuery( GLuint queryID );
void GetTimerQueryResults( float *resultArray );


#endif

