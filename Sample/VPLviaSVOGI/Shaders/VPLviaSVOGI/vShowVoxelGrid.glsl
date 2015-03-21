#version 430 core

in vec4 vPosition;
in vec3 vNormal;

out vec4 vPositionWorld;
out vec4 vNormalWorld;

struct GatheredVoxel
{
    vec4 translation;
};

layout(std430, binding = 1)  buffer _indirectCommandBuffer
{
    uint  count;
    uint  primCount;
    uint  firstIndex;
    uint  baseVertex;
    uint  baseInstance;

    float reserved1;
    float reserved2;
    float reserved3;
    float reserved4;
    float reserved5;
    float reserved6;
    float reserved7;
    float reserved8;
    float reserved9;
    float reserved10;
    float reserved11;
    float reserved12;
    float reserved13;
    float reserved14;
    float reserved15;
    float reserved16;
    float reserved17;
    float reserved18;
    float reserved19;
    float reserved20;
    float reserved21;
    float reserved22;
    float reserved23;
    float reserved24;
    float reserved25;
    float reserved26;
    float reserved27;
    float reserved28;
    float reserved29;
    float reserved30;
    float reserved31;
    float reserved32;
    float reserved33;
    float reserved34;
    float reserved35;

    GatheredVoxel data[];
} indirectCommandBuffer;

uniform mat4 View;
uniform mat4 Proj;

mat4 GetWorldTransform(int instanceID)
{
    mat4 res;

    vec3 scale = vec3(indirectCommandBuffer.reserved1,
                      indirectCommandBuffer.reserved2,
                      indirectCommandBuffer.reserved3);
    scale *= 0.5; // Because we are using cube of length 2.
    vec3 trans = vec3(indirectCommandBuffer.data[instanceID].translation.x,
                      indirectCommandBuffer.data[instanceID].translation.y,
                      indirectCommandBuffer.data[instanceID].translation.z);

    res = mat4(vec4(scale.x, 0.0, 0.0, 0.0),
               vec4(0.0, scale.y, 0.0, 0.0),
               vec4(0.0, 0.0, scale.z, 0.0),
               vec4(trans.x, trans.y, trans.z, 1.0));

    return res;
}

void main()
{
    mat4 World = GetWorldTransform(gl_InstanceID);
    vPositionWorld = World * vPosition;
    vNormalWorld = vec4(vNormal, 0.0);
    gl_Position = Proj * View * vPositionWorld;
}