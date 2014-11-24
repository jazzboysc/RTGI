#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Voxel
{
    uint value1;
    uint value2;
    uint value3;
    uint value4;
};

layout(std430, binding = 0)  buffer _voxelBuffer
{
    int test;
    Voxel data[];
} voxelBuffer;

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform int dim;

uniform vec3 RayStartPoint;
uniform vec3 RayEndPoint;

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

void main()
{
    vec3 RayDirection = RayEndPoint - RayStartPoint;
    RayDirection = normalize(RayDirection);
    vec3 gridIntersect = RayStartPoint;
    vec3 voxelExtension = 2.0*SceneBBExtension / dim;

    // Set up 3D DDA for ray.
    vec3 NextCrossingT, DeltaT;
    ivec3 Step, EndPos, CurPos;
    const int cmpToAxis[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
    CurPos = GetGridPosition(gridIntersect);
    EndPos = GetGridPosition(RayEndPoint);
    for( int axis = 0; axis < 3; ++axis )
    {
        if( RayDirection[axis] >= 0 )
        {
            NextCrossingT[axis] = 
                (GetWorldPosition(CurPos[axis] + 1, axis) - gridIntersect[axis]) / RayDirection[axis];
            DeltaT[axis] = voxelExtension[axis] / RayDirection[axis];
            Step[axis] = 1;
        }
        else
        {
            NextCrossingT[axis] =
                (GetWorldPosition(CurPos[axis], axis) - gridIntersect[axis]) / RayDirection[axis];
            DeltaT[axis] = -voxelExtension[axis] / RayDirection[axis];
            Step[axis] = -1;
        }
    }

    // Walk ray through voxel grid.
    bool hitSomething = false;
    while( true )
    {
        int index = GetIndex(CurPos);
        Voxel voxel = voxelBuffer.data[index];
        if( voxel.value2 == 1 )
        {
            hitSomething = true;
            break;
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

    if( hitSomething )
    {
        voxelBuffer.test = 1;
    }
    else
    {
        voxelBuffer.test = 0;
    }
}