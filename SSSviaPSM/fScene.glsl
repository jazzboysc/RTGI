#version 430 core

in vec4 vNormalWorld;
in vec4 vPositionWorld;

uniform mat4 LightProjectorView;
uniform vec3 LightPositionWorld;
uniform vec3 CameraPositionWorld;
uniform vec3 LightColor;
uniform vec3 MaterialColor;
uniform vec2 LightProjectorNearFar;
uniform bool IsSSS;

uniform sampler2D shadowMapSampler;

void main()
{
    vec3 normal = normalize(vNormalWorld).xyz;

    bool skipShadow = false;
    vec4 viewPos = LightProjectorView * vPositionWorld;
    if( viewPos.z > 0.0 )
    {
        skipShadow = true;
    }
    vec3 viewDir = viewPos.xyz;
    float len = length(viewDir);
    viewDir = normalize(viewDir);
    vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
    float u = -halfDir.x / halfDir.z;
    float v = -halfDir.y / halfDir.z;
    float currDepth = (len - LightProjectorNearFar.x) /
        (LightProjectorNearFar.y - LightProjectorNearFar.x);

    vec2 texCoords = vec2(u, v);
    texCoords = texCoords*0.5 + 0.5;
    float depth = texture2D(shadowMapSampler, texCoords).r;
    float diff = currDepth - depth;

    if( IsSSS )
    {
        vec3 lightDir = LightPositionWorld - vPositionWorld.xyz;
        lightDir = normalize(lightDir);
        vec3 viewDir = CameraPositionWorld - vPositionWorld.xyz;
        viewDir = normalize(viewDir);

        float NdL = dot(normal, lightDir);
        float VdML = dot(viewDir, -lightDir);

        vec3 color = exp(-diff*10.0) * MaterialColor;
        gl_FragData[0] = vec4(color, 1.0);
    }
    else
    {
        if( diff > 0.01 && !skipShadow )
        {
            // Shadow.
            gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else
        {
            // Direct lighting.
            vec3 lightDir = LightPositionWorld - vPositionWorld.xyz;
            lightDir = normalize(lightDir);
            float d = max(0.0, dot(lightDir, normal));
            vec3 color = MaterialColor * d;
            gl_FragData[0] = vec4(color, 1.0);
        }
    }
}
