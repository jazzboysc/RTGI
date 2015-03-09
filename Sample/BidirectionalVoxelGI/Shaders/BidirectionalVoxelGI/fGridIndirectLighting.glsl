#version 430 core

in vec2 pTCoord;

out vec4 Output;

#define PI 3.141593

uniform int VPLCount;
uniform int PatternSize;
uniform float BounceSingularity;
uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;
uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform int dim;
uniform bool VPLVisibilityTest;

struct VPL
{
    vec4 WorldPosition;
    vec4 WorldNormal;
    vec4 Flux;
    mat4 View;
};

layout(std430, binding = 0)  buffer _VPLBuffer
{
    VPL vpls[];
} VPLBuffer;

struct Voxel
{
    uint value1;
    uint value2;
    uint value3;
    uint value4;
};

layout(std430, binding = 1)  buffer _voxelBuffer
{
    Voxel data[];
} voxelBuffer;

ivec3 GetGridPosition(vec3 worldPosition)
{
    vec3 imageDim = vec3(float(dim), float(dim), float(dim));
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBCenter + SceneBBExtension) / (2.0*SceneBBExtension);
    offsets = offsets*imageDim;
    ivec3 res = ivec3(offsets);

    return res;
}

float GetWorldPosition(int gridPosition, int axis)
{
    float voxelExtension = 2.0*SceneBBExtension[axis] / dim;
    float sceneBBMin = SceneBBCenter[axis] - SceneBBExtension[axis];
    float worldPosition = sceneBBMin + float(gridPosition)*voxelExtension;
    return worldPosition;
}

int GetIndex(ivec3 gridPosition)
{
    int index = gridPosition.z * dim * dim + gridPosition.y * dim + gridPosition.x;
    return index;
}

bool VoxelGridIntersectionTest(vec3 rayStartPoint, vec3 rayEndPoint)
{
    vec3 RayDirection = rayEndPoint - rayStartPoint;
    float maxT = length(RayDirection);
    RayDirection = RayDirection / maxT;
    // Avoid self-intersection.
    vec3 gridIntersect = rayStartPoint + RayDirection*0.125*maxT;
    rayEndPoint = rayEndPoint - RayDirection*0.125*maxT;
    vec3 voxelExtension = 2.0*SceneBBExtension / dim;

    // Set up 3D DDA for ray.
    vec3 NextCrossingT, DeltaT;
    ivec3 Step, StartPos, EndPos, CurPos;
    const int cmpToAxis[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
    StartPos = GetGridPosition(gridIntersect);
    EndPos = GetGridPosition(rayEndPoint);
    CurPos = StartPos;

    vec3 tempWorldPosition;
    for( int axis = 0; axis < 3; ++axis )
    {
        if( RayDirection[axis] >= 0 )
        {
            tempWorldPosition[axis] = GetWorldPosition(CurPos[axis] + 1, axis);
            NextCrossingT[axis] = (tempWorldPosition[axis] - gridIntersect[axis]) / RayDirection[axis];
            DeltaT[axis] = voxelExtension[axis] / RayDirection[axis];
            Step[axis] = 1;
        }
        else
        {
            tempWorldPosition[axis] = GetWorldPosition(CurPos[axis], axis);
            NextCrossingT[axis] = (tempWorldPosition[axis] - gridIntersect[axis]) / RayDirection[axis];
            DeltaT[axis] = -voxelExtension[axis] / RayDirection[axis];
            Step[axis] = -1;
        }

        if( isinf(NextCrossingT[axis]) )
        {
            NextCrossingT[axis] = 1000.0*maxT;
        }
        if( isinf(DeltaT[axis]) )
        {
            DeltaT[axis] = 1000.0*maxT;
        }
    }

    // Walk ray through voxel grid.
    while( true )
    {
        int index = GetIndex(CurPos);
        Voxel voxel = voxelBuffer.data[index];
        if( voxel.value2 == 1 )
        {
            return true;
        }

        // Advance to next voxel.
        int c1 = int(NextCrossingT[0] < NextCrossingT[1]);
        int c2 = int(NextCrossingT[0] < NextCrossingT[2]);
        int c3 = int(NextCrossingT[1] < NextCrossingT[2]);

        int bits = (c1 << 2) + (c2 << 1) + c3;
        int stepAxis = cmpToAxis[bits];

        CurPos[stepAxis] += Step[stepAxis];
        if( CurPos[stepAxis] == EndPos[stepAxis] )
        {
            break;
        }
        NextCrossingT[stepAxis] += DeltaT[stepAxis];
    }

    return false;
}

void main()
{
    vec4 PositionWorld = texture(GBufferPositionSampler, pTCoord);
    if( PositionWorld.w == 0.0 )
    {
        discard;
    }

    vec3 NormalWorld = texture(GBufferNormalSampler, pTCoord).rgb;
    NormalWorld = NormalWorld*2.0 - 1.0;

    vec4 MaterialColor = texture(GBufferAlbedoSampler, pTCoord);

    int patternIndex = int(gl_FragCoord.x) % PatternSize + (int(gl_FragCoord.y) % PatternSize) * PatternSize;
    int sampleVPLCount = VPLCount / (PatternSize*PatternSize);
    int vplBufferIndex = sampleVPLCount * patternIndex;

    vec3 indirectColor = vec3(0.0, 0.0, 0.0);
    for( int i = 0; i < sampleVPLCount; ++i )
    {
        VPL vpl = VPLBuffer.vpls[vplBufferIndex + i];

        if( VPLVisibilityTest )
        {
            bool isInShadow = VoxelGridIntersectionTest(PositionWorld.xyz, vpl.WorldPosition.xyz);
            if( isInShadow )
            {
                continue;
            }
        }

        vpl.WorldNormal = vpl.WorldNormal*2.0 - 1.0;

        vec3 incidentDir = PositionWorld.xyz - vpl.WorldPosition.xyz;
        float len = length(incidentDir);
        incidentDir = incidentDir / len;

        float cos0 = max(0.0, dot(NormalWorld, -incidentDir));
        float cos1 = max(0.0, dot(incidentDir, vpl.WorldNormal.xyz));
        float geometricTerm = cos0 * cos1 / max(len*len, BounceSingularity);

        indirectColor += MaterialColor.rgb * vpl.Flux.rgb * geometricTerm;
    }

    indirectColor = indirectColor * 2 * PI;
    Output = vec4(indirectColor, 1.0);
}
