struct Voxel
{
    uint value1;
    uint value2;
    uint value3;
    uint value4;
};

struct GatheredVoxel
{
    vec4 translation;
};

layout(std430, binding = 0)  buffer _voxelBuffer
{
    Voxel data[];
} voxelBuffer;

layout(std430, binding = 2)  buffer _indirectCommandBuffer
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