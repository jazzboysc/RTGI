#version 410 core

in vec4 vNormalWorld;
in vec4 vPositionWorld;

uniform mat4 LightProjectorView;
uniform vec3 LightPositionWorld;
uniform vec3 LightColor;
uniform vec3 MaterialColor;
uniform vec2 LightProjectorNearFar;

uniform sampler2D shadowMapSampler;

void main()
{
    vec3 normal = normalize(vNormalWorld).xyz;
    vec3 test = LightPositionWorld - vPositionWorld.xyz;
    float testLen = length(test);
    if( testLen <= 0.1 )
    {
        gl_FragData[0] = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
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

        if( currDepth - depth > 0.01 && !skipShadow )
        {
            // Indirect shadow.
            gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else
        {
            // Indirect lighting.
            vec3 lightNormal = vec3(-1.0, 0.0, 0.0);
            vec3 lightDir = LightPositionWorld - vPositionWorld.xyz;
            float len = length(lightDir);
            lightDir = lightDir / len;
            float d0 = max(0.0, dot(lightDir, normal));
            float d1 = max(0.0, dot(-lightDir, lightNormal));
            vec3 color = MaterialColor * LightColor * d0 * d1 / len;
            gl_FragData[0] = vec4(color, 1.0);
        }
    }
}
