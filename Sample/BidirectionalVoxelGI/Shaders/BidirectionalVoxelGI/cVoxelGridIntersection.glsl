#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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
    float maxT = length(RayDirection);
    RayDirection = RayDirection / maxT;
    vec3 gridIntersect = RayStartPoint;
    vec3 voxelExtension = 2.0*SceneBBExtension / dim;

    // Debug RayDirection, voxelExtension, maxT.
    indirectCommandBuffer.reserved5 = float(RayDirection[0]);
    indirectCommandBuffer.reserved6 = float(RayDirection[1]);
    indirectCommandBuffer.reserved7 = float(RayDirection[2]);
    indirectCommandBuffer.reserved8 = float(voxelExtension[0]);
    indirectCommandBuffer.reserved9 = float(voxelExtension[1]);
    indirectCommandBuffer.reserved10 = float(voxelExtension[2]);
    indirectCommandBuffer.reserved11 = maxT;

    // Set up 3D DDA for ray.
    vec3 NextCrossingT, DeltaT;
    ivec3 Step, StartPos, EndPos, CurPos;
    const int cmpToAxis[8] = { 2, 1, 2, 1, 2, 2, 0, 0 };
    StartPos = GetGridPosition(gridIntersect);
    EndPos = GetGridPosition(RayEndPoint);
    CurPos = StartPos;

    // Debug StartPos, EndPos.
    indirectCommandBuffer.reserved12 = float(StartPos[0]);
    indirectCommandBuffer.reserved13 = float(StartPos[1]);
    indirectCommandBuffer.reserved14 = float(StartPos[2]);
    indirectCommandBuffer.reserved15 = float(EndPos[0]);
    indirectCommandBuffer.reserved16 = float(EndPos[1]);
    indirectCommandBuffer.reserved17 = float(EndPos[2]);

    vec3 tempWorldPosition;
    for( int axis = 0; axis < 3; ++axis )
    {
        if( RayDirection[axis] >= 0 )
        {
            tempWorldPosition[axis] = GetWorldPosition(CurPos[axis] + 1, axis);
            NextCrossingT[axis] = (tempWorldPosition[axis] - gridIntersect[axis]) / RayDirection[axis];
            DeltaT[axis] = voxelExtension[axis] / RayDirection[axis];
            Step[axis] = 1;

            // Debug.
            //if( axis == 1 )
            //{
            //    indirectCommandBuffer.reserved18 = tempWorldPosition[axis];
            //    indirectCommandBuffer.reserved19 = gridIntersect[axis];
            //    indirectCommandBuffer.reserved20 = NextCrossingT[axis];
            //    indirectCommandBuffer.reserved21 = DeltaT[axis];
            //}
        }
        else
        {
            tempWorldPosition[axis] = GetWorldPosition(CurPos[axis], axis);
            NextCrossingT[axis] = (tempWorldPosition[axis] - gridIntersect[axis]) / RayDirection[axis];
            DeltaT[axis] = -voxelExtension[axis] / RayDirection[axis];
            Step[axis] = -1;

            // Debug.
            //if( axis == 1 )
            //{
            //    indirectCommandBuffer.reserved18 = tempWorldPosition[axis];
            //    indirectCommandBuffer.reserved19 = gridIntersect[axis];
            //    indirectCommandBuffer.reserved20 = NextCrossingT[axis];
            //    indirectCommandBuffer.reserved21 = DeltaT[axis];
            //}
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

    // Debug.
    //indirectCommandBuffer.reserved18 = NextCrossingT[0];
    //indirectCommandBuffer.reserved19 = NextCrossingT[1];
    //indirectCommandBuffer.reserved20 = NextCrossingT[2];
    //indirectCommandBuffer.reserved21 = DeltaT[0];
    //indirectCommandBuffer.reserved22 = DeltaT[1];
    //indirectCommandBuffer.reserved23 = DeltaT[2];
    //indirectCommandBuffer.reserved24 = float(Step[0]);
    //indirectCommandBuffer.reserved25 = float(Step[1]);
    //indirectCommandBuffer.reserved26 = float(Step[2]);

    // Walk ray through voxel grid.
    indirectCommandBuffer.reserved4 = 0.0;
    int i = 0;
    while( true )
    {
        int index = GetIndex(CurPos);
        Voxel voxel = voxelBuffer.data[index];
        if( voxel.value2 == 1 )
        {
            // Debug.
            indirectCommandBuffer.reserved18 = float(CurPos[0]);
            indirectCommandBuffer.reserved19 = float(CurPos[1]);
            indirectCommandBuffer.reserved20 = float(CurPos[2]);
            indirectCommandBuffer.reserved21 = float(i);

            indirectCommandBuffer.reserved4 = 123.0;
            break;
        }

        // Advance to next voxel.
        int c1 = int(NextCrossingT[0] < NextCrossingT[1]);
        int c2 = int(NextCrossingT[0] < NextCrossingT[2]);
        int c3 = int(NextCrossingT[1] < NextCrossingT[2]);
        //int c1 = 0;
        //int c2 = 0;
        //int c3 = 0;
        //if( NextCrossingT[0] < NextCrossingT[1] )
        //{
        //    c1 = 1;
        //}
        //if( NextCrossingT[0] < NextCrossingT[2] )
        //{
        //    c2 = 1;
        //}
        //if( NextCrossingT[1] < NextCrossingT[2] )
        //{
        //    c3 = 1;
        //}

        int bits = (c1 << 2) + (c2 << 1) + c3;
        int stepAxis = cmpToAxis[bits];

        // Debug.
        //if( i == 0 )
        //{
        //    indirectCommandBuffer.reserved27 = float(c1);
        //    indirectCommandBuffer.reserved28 = float(c2);
        //    indirectCommandBuffer.reserved29 = float(c3);
        //    indirectCommandBuffer.reserved30 = float(bits);
        //    indirectCommandBuffer.reserved31 = float(stepAxis);
        //    indirectCommandBuffer.reserved32 = NextCrossingT[0];
        //    indirectCommandBuffer.reserved33 = NextCrossingT[1];
        //    indirectCommandBuffer.reserved34 = NextCrossingT[2];
        //}

        CurPos[stepAxis] += Step[stepAxis];
        if( CurPos[stepAxis] == EndPos[stepAxis] )
        {
            break;
        }
        NextCrossingT[stepAxis] += DeltaT[stepAxis];

        i++;
    }
}