#ifndef RTGI_BuildSVO_H
#define RTGI_BuildSVO_H

#include "ComputeTask.h"

#define BUILD_SVO_INIT_ROOT_PASS        0
#define BUILD_SVO_FLAG_NODES_PASS       1
#define BUILD_SVO_ALLOC_NODES_PASS      2
#define BUILD_SVO_POST_ALLOC_NODES_PASS 3
#define BUILD_SVO_INIT_NODES_PASS       4
#define BUILD_SVO_SPLAT_LEAF_NODES_PASS 5

namespace RTGI
{

//----------------------------------------------------------------------------
// These structs are used for debugging SVO node buffer.
//----------------------------------------------------------------------------
struct SVONodeAABB
{
    unsigned int Min;
    unsigned int Max;
};
//----------------------------------------------------------------------------
struct SVONode
{
    unsigned int child;
    unsigned int flag;
    unsigned int isLeaf;
    unsigned int albedo;

    SVONodeAABB nodeBox;
};
//----------------------------------------------------------------------------
struct SVONodeTile
{
    SVONode children[8];
};
//----------------------------------------------------------------------------
struct SVONodeBufferHead
{
    // Indirect command buffer data for allocation of SVO node tile pass.
    unsigned int  allocThreadCountForCurLevel;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;

    // Indirect command buffer data for visualizing SVO nodes.
    unsigned int  ic2Count;
    unsigned int  ic2PrimCount;
    unsigned int  ic2FirstIndex;
    unsigned int  ic2BaseVertex;
    unsigned int  ic2BaseInstance;

    unsigned int offset0;
    unsigned int offset1;
    unsigned int offset2;

    // Shared data between SVO passes.
    unsigned int rootFlag;
    unsigned int rootChild;
    unsigned int curLevelStartIndex;
    unsigned int curLevelEndIndex;

    // Debug.
    unsigned int _hit;
    float _minT;
    float _maxT;
    float _sceneMaxT;
    unsigned int _isLeaf;
    unsigned int _childIndex;
    unsigned int _childTileIndex;
    unsigned int _nextNodeIndex;
    float _tNear;
    float _tFar;
    float _minMinusOriginK;
    float _maxMinusOriginK;
    uint _tNearCompareTFar;
    uint _b;
    uint _c;
    uint _d;
    vec4 _sceneBBMin;
    vec4 _rayStartPos;
    vec4 _rayStartPosSVO;
    vec4 _rayEndPos;
    vec4 _rayEndPosSVO;
    vec4 _rayEntryPos;
    vec4 _nodeBoxMin;
    vec4 _nodeBoxMax;
    vec4 _mid;
    vec4 _rayInvDirSVO;
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Voxel fragment list buffer.
//----------------------------------------------------------------------------
struct VoxelFragment
{
    unsigned int gridPosition;
    unsigned int albedo;
    unsigned int value1;
    unsigned int value2;
};
//----------------------------------------------------------------------------
struct VoxelFragmentBufferHead
{
    // Indirect command buffer data for SVO flag nodes pass.
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;

    // Scene bounding box.
    vec4 SceneBBMin;
    vec4 SceneBBCenter;
    vec4 SceneBBExtension;
    vec4 Inv2SceneBBExtension;
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/27/2015
//----------------------------------------------------------------------------
class BuildSVO : public ComputeTask
{
public:
    BuildSVO();
    ~BuildSVO();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVO> BuildSVOPtr;

}

#endif