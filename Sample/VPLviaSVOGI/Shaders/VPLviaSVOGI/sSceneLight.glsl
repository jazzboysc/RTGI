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
    SceneLight lights[];
} sceneLightUniformBuffer;