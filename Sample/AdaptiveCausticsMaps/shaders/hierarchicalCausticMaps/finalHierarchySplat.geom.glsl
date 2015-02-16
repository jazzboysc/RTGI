// This "splatting" shader actually takes as input screen-space
//    positions that are not yet fully refined.  In fact they
//    reference locations in mipmap level 1 (up 1 level from 
//    the finest subdivision).  This shader checks if each of
//    these 2x2 clusters is valid.  If not, it returns.
//    If the cluster *is* valid, it checks if the photons 
//    converge in the caustic map, at which case a single splat
//    with 4x the intensity is significantly cheaper than 
//    rasterising all 4 child photons.
// The output of this shader is either 0, 1, or 4 photons or
//    photon "clusters" which are rasterized and then actually
//   splatted into the caustic map in the fragment shader.

#version 120 
#extension GL_EXT_gpu_shader4 : enable
#extension GL_EXT_geometry_shader4 : enable

#define DEFAULT_POINT_SIZE  2.5

// Photon positions is the photon buffer.  Light color is
//    the spotlight texture.  Indexing into it here reduces 
//    the texture lookups from one per splat fragment (if done
//    in the fragment shader) to one per splat (if done here)
uniform sampler2D photonPositions, lightColor;

// This is the x/y resolution of the photon buffer.
//   Obviously, if this is non-square, more than 1 
//   value will be needed.  We assume square photon buf.
uniform vec2 photonRes, causticMapRes;

void main( void )
{
	// Check the validity of our 2x2 photon cluster.
	vec4 eyeTmp;
	vec4 mipMap = texture2DLod( photonPositions, gl_PositionIn[0].xy, 1.0f );
	if( mipMap.a == -10.0 ) return;
		
	// OK.  Get the 4 photon positions.  Find an average position.
	vec4 position[4];
	position[0] = texture2D( photonPositions, gl_PositionIn[0].xy );
	position[1] = texture2D( photonPositions, gl_PositionIn[0].xy+vec2(0,photonRes.y) );
	position[2] = texture2D( photonPositions, gl_PositionIn[0].xy+vec2(photonRes.y,0) );
	position[3] = texture2D( photonPositions, gl_PositionIn[0].xy+vec2(photonRes.y,photonRes.y) );
	vec4 sumPos = position[0]+position[1]+position[2]+position[3];
	sumPos.xyz *= 0.25;
	
	// We can save even more time on the spotlight by only looking up the spot
	//   texture once per cluster instead of once per photon.  And really, this
	//   doesn't usually matter (unless index of refraction = 1, in which case
	//   why are you computing caustics?)
	vec4 mipColor = texture2DLod( lightColor, gl_PositionIn[0].xy, 1.0f );
				
	// Check for convergence.  The names are bad.  Holdovers from prior experiments.
	//   lensUnrefracted is the threshold where photons converge (i.e., their separation
	//   at index of refraction = 1.0).
	vec3 lenUnrefracted = vec3( 1.570796 * length( position[0].xyz ) * causticMapRes.y );
	vec3 lenRefracted;
	lenRefracted.x = length(position[1].xyz - position[0].xyz);
	lenRefracted.y = length(position[2].xyz - position[0].xyz);
	lenRefracted.z = length(position[3].xyz - position[0].xyz);
	 
	// These are really small!  Combine them into 1 splat with a 4x weight! 
	//   We check the alpha component too, because we don't want to combine
	//   photons if some are still invalid!
	if (all(lessThan(lenRefracted,lenUnrefracted)) && sumPos.a >= 0.0)
	{
		//vec4 posSum = 0.25*(sumPos);
		//posSum.w = 1;
		eyeTmp = gl_ModelViewProjectionMatrix * sumPos;
		eyeTmp /= eyeTmp.w;
		gl_Position = eyeTmp;
		gl_TexCoord[4] = eyeTmp * 0.5 + 0.5; 
		gl_TexCoord[4].z =  DEFAULT_POINT_SIZE;
		gl_PointSize =  DEFAULT_POINT_SIZE;
		gl_TexCoord[4].w = 4;
		gl_TexCoord[5] = vec4( 0 );  // The 'offset' used in the frag shader to multi-sample the gaussian.
		gl_TexCoord[6] = mipColor;
		gl_Layer = 0;
		EmitVertex();
		EndPrimitive();
		return;
	}
	
		
	// Determine the best point size to fit all the photon hit points.
	vec3 ptSize = lenRefracted / lenUnrefracted;
	float maxDir = max(1.0, max( ptSize.x, max( ptSize.y, ptSize.z ) )) + 1.5;
	
	// We're going to select the layer of the hierarchical caustic map to splat into.
	//    this will be triggered any time a point is bigger than 8 pixels wide.  There's
	//    two metrics here.  The first was my initial implmentation (with a 5 pixel threshold)
	//    that actually works better but is actually completely bogus (i.e., not based on
	//    any physical reality I can see).  I actually thought I was doing the second algorithm,
	//    discussed in the HCM paper, when I wrote it.  Oops.  But it still works better!
	
	//int currLayer = int( log( maxDir+0.01 )/ log( 5.0 ) ); 
	int currLayer = int( floor( log2( max( 1.0, (maxDir+0.01)/8.0 ) ) ) );
	
	// Since we're not really rendering-to-mipmap, we need to adjust the screen position
	//   inside the layer we're rendering to (otherwise splats will be way too far apart).
	float posMult = pow( 0.5, currLayer );  
	maxDir *= posMult;                        // Update the point size based on the current level                      
	
	// If the ``photon'' is really quite big and will thus be distributed over a huge area.
	//    perhaps we should just ignore it, and lose it's tiny contribution -- save time!
	//    Plus, then we only need 4 layers.
	if (currLayer > 3) return;
	if (currLayer > 0 && sumPos.a < 0.0 ) return;
	
	// Setup values consistant over all the photons.
	gl_Layer = currLayer;
	gl_PointSize = maxDir;
	float fragCoordOffset = 0.5f * causticMapRes.y / posMult; 
	gl_TexCoord[5] = vec4( fragCoordOffset, fragCoordOffset, fragCoordOffset, -fragCoordOffset );
	gl_TexCoord[4].z = maxDir;
	gl_TexCoord[4].w = posMult * posMult;
		
	// Photon #0
	if (position[0].a > 0 )
	{
		eyeTmp = gl_ModelViewProjectionMatrix * position[0];
		eyeTmp /= eyeTmp.w;	
		//eyeTmp.xy = 2 * posMult * (eyeTmp.xy * 0.5 + 0.5) - 1;
		eyeTmp.xy = posMult * (eyeTmp.xy + 1) - 1;
		gl_Position = eyeTmp;
		gl_TexCoord[4].xy = eyeTmp.xy * 0.5 + 0.5;
		gl_TexCoord[6] = texture2D( lightColor, gl_PositionIn[0].xy );
		EmitVertex();
		EndPrimitive();
	}
	
	// Photon #1
	if (position[1].a > 0)
	{
		eyeTmp = gl_ModelViewProjectionMatrix * position[1];
		eyeTmp /= eyeTmp.w;
		eyeTmp.xy = posMult * (eyeTmp.xy + 1) - 1;
		gl_Position = eyeTmp;
		gl_TexCoord[4].xy = eyeTmp.xy * 0.5 + 0.5;
		gl_TexCoord[6] = texture2D( lightColor, gl_PositionIn[0].xy+vec2(0,photonRes.y) );
		EmitVertex();
		EndPrimitive();
	}
	
	// Photon #2
	if (position[2].a > 0)
	{
		eyeTmp = gl_ModelViewProjectionMatrix * position[2];
		eyeTmp /= eyeTmp.w;
		eyeTmp.xy = posMult * (eyeTmp.xy + 1) - 1;
		gl_Position = eyeTmp;
		gl_TexCoord[4].xy = eyeTmp.xy * 0.5 + 0.5 ; 
		gl_TexCoord[6] = texture2D( lightColor, gl_PositionIn[0].xy+vec2(photonRes.y,0) );
		EmitVertex();
		EndPrimitive();
	}
	
	// Photon #3
	if (position[3].a > 0)
	{
		eyeTmp = gl_ModelViewProjectionMatrix * position[3];
		eyeTmp /= eyeTmp.w;
		eyeTmp.xy = posMult * (eyeTmp.xy + 1) - 1;
		gl_Position = eyeTmp;
		gl_TexCoord[4].xy = eyeTmp.xy * 0.5 + 0.5 ; 
		gl_TexCoord[6] = texture2D( lightColor, gl_PositionIn[0].xy+vec2(photonRes.y,photonRes.y) );
		EmitVertex();
		EndPrimitive();
	}
}
