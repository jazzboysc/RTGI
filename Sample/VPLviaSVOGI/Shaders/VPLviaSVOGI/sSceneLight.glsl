#define MAX_LIGHT_COUNT 256

struct LightBufferHead
{
    uint PointLightCount;
    uint SpotLightCount;
    uint Reserved1;
    uint Reserved2;
};

struct ShadowMapInfo
{
    uint CurLightIndex;
    uint Reserved1;
    uint Reserved2;
    uint Reserved3;
};

//----------------------------------------------------------------------------
// Scene Light. Currently support point and spot lights.
//----------------------------------------------------------------------------
struct SceneLight
{
    vec4 WorldPositionAndType; // type 1: point light, type 2: spot light
    vec4 Intensity;

    vec4 Params1; // SpotDirection, CosCutoff
    vec4 Params2; // SpotExponent, ConstantAttenuation, QuadraticAttenuation, SpotInnerCosCutoff

    mat4 LightProjectorView;
    mat4 LightProjectorProj;
    vec4 LightProjectorNearFar;
};

layout(std140, binding = 1) uniform _sceneLightUniformBuffer
{
    LightBufferHead info;
    SceneLight lights[MAX_LIGHT_COUNT];
} sceneLightUniformBuffer;

layout(std140, binding = 2) uniform _shadowMapUniformBuffer
{
    ShadowMapInfo info;
} shadowMapUniformBuffer;

uniform bool ShowShadow;
uniform sampler2DArray ShadowMapSampler;

//----------------------------------------------------------------------------
vec4 ComputePointLight(int i, vec4 PositionWorld, vec3 NormalWorld, vec4 Material)
{
    vec4 res;

    SceneLight light = sceneLightUniformBuffer.lights[i];

    // Compare with shadow map bias.
    bool skipShadow = false;
    vec4 viewPos = light.LightProjectorView * PositionWorld;
    if( viewPos.z > -0.05 )
    {
        skipShadow = true;
    }
    vec3 viewDir = viewPos.xyz;
    float len = length(viewDir);
    viewDir = normalize(viewDir);
    vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
    float u = -halfDir.x / halfDir.z;
    float v = -halfDir.y / halfDir.z;
    float currDepth = (len - light.LightProjectorNearFar.x) /
        (light.LightProjectorNearFar.y - light.LightProjectorNearFar.x);

    vec2 texCoords = vec2(u, v);
    texCoords = texCoords*0.5 + 0.5;
    float depth = texture(ShadowMapSampler, vec3(texCoords, float(i))).r;

    if( currDepth - depth > 0.01 && !skipShadow && ShowShadow )
    {
        // Shadow.
        res = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // Direct lighting.
        vec3 lightDir = light.WorldPositionAndType.xyz - PositionWorld.xyz;
        float len = length(lightDir);
        lightDir = lightDir / len;
        float d = max(0.0, dot(lightDir, NormalWorld));
        vec3 color = Material.rgb * light.Intensity.xyz * d / (1.0 + 0.2*len*len);
        res = vec4(color, 1.0);
    }

    return res;
}
//----------------------------------------------------------------------------
vec4 ComputeSpotLight(int i, vec4 PositionWorld, vec3 NormalWorld, vec4 Material)
{
    vec4 res = vec4(0.0);
    return res;
}
//----------------------------------------------------------------------------