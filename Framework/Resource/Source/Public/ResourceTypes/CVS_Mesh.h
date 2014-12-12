#pragma once
#include "CVS_Resource.h"

const unsigned char MAX_BONES_PER_VERTEX = 6;
const unsigned char MAX_BONES = 100;

class CVS_Skeleton;

// For every material, record the offsets in every VBO and triangle counts
struct CVS_SubMesh
{
	CVS_SubMesh() : m_indexOffset(0), m_triangleCount(0) {}

	int m_indexOffset;
	int m_triangleCount;
};

struct VertexBoneData
{
	unsigned int BoneIDs[MAX_BONES_PER_VERTEX];
	float BoneWeights[MAX_BONES_PER_VERTEX];

	void AddBoneData(unsigned char BoneID, float Weight)
	{
		for (unsigned char i = 0; i < (sizeof(BoneIDs) / sizeof(BoneIDs[0])); i++)
		{
			if (BoneWeights[i] == 0.0f ||
				BoneWeights[i] == 1.0f /*Default 1.0f and Bone# 0*/)
			{
				BoneIDs[i] = BoneID;
				BoneWeights[i] = Weight;
				return;
			}
		}

		// should never get here - more bones than we have space for
		assert(0);
	}
	VertexBoneData() : BoneIDs(), BoneWeights()
	{
		BoneWeights[0] = 1.0f;
	}
};

struct CVS_Mesh : public CVS_Resource
{

	std::vector<CVS_SubMesh*> m_subMeshes;
	CVS_Skeleton* m_pSkeleton;

	int mVertexCount; 
	int mFaceCount;
	int mVertexComponentCount;
	std::vector<glm::vec3> mVertexData;
	std::vector<glm::vec2> mTCoordData;
	std::vector<glm::vec3> mVertexNormalData;
	std::vector<glm::vec3> mFaceNormalData;
	std::vector<VertexBoneData> mBoneData;
	std::vector<unsigned short> mIndexData;
	
	glm::mat4 m_offset;
	GLuint m_VAO;

	void Init();
	CVS_Mesh() : CVS_Resource("Mesh", CVS_Resource::EType::Mesh), m_pSkeleton(), m_VAO() {}
};
