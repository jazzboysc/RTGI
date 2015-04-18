#version 430

//  We're going to output the object's surface normal
//     in the fragment shader, so make sure to pass down
//     the eye-space surface normal to the fragment level.
//
//  However, since we're going to output BOTH front and back
//     facing surfaces in the same pass, we need to determine
//     if this certex is front-facing or back-facing 
//     (the dot produt)

in vec4 vPosition;
in vec3 vNormal;

out vec4 vPositionWorld;
out vec4 vPositionView;
out vec4 vPositionProj;

out vec4 vNormalWorld;
out vec4 vNormalView;

out float distanceView;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalView = transpose(inverse(View * World)) * vec4(vNormal, 0.0);
	vNormalWorld = World * vec4(vNormal, 0.0);
	vPositionWorld = World * vPosition;
	vPositionView = View * vPositionWorld;
	vPositionProj = Proj * vPositionView;

	vNormalView.w = dot( normalize(vNormalView.xyz), -normalize(vPositionView.xyz) );
	distanceView = length( vPositionView.xyz );

    gl_Position = vPositionProj;
}



