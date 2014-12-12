#pragma once
#include "CVS_Resource.h"
class CVS_Skeleton;

struct CVS_BonePose
{
	enum EInterpolationType
	{
		eInterpolationConstant = 0x00000002,	//!< Constant value until next key.
		eInterpolationLinear = 0x00000004,		//!< Linear progression to next key.
		eInterpolationCubic = 0x00000008		//!< Cubic progression to next key.
	};

	// Current SRT
	glm::quat m_rotation;
	glm::vec3 m_translation;
	glm::vec3 m_scaling;

	char m_interpolationType;
	//char m_padding[8];
	CVS_BonePose() : m_scaling(), m_rotation(), m_translation()
	{}

	CVS_BonePose(glm::vec3 _s, glm::quat _r, glm::vec3 _t) : m_scaling(_s), m_rotation(_r), m_translation(_t)
	{}

	glm::mat4 toMat4()
	{
		return glm::translate(glm::mat4(), m_translation) *	glm::mat4_cast(m_rotation) * glm::scale(glm::mat4(), m_scaling);
	}
};

struct CVS_Bone : public CVS_InternalResource
{
	// Current SRT
	glm::quat m_rotation;
	glm::vec3 m_translation;
	glm::vec3 m_scaling;

	//CVS_Bone* m_pParent;
	short m_parentIndex;
	std::vector<CVS_Bone*> m_children;

	char m_index;
	glm::vec3 m_color;

	glm::mat4 m_bindPoseInv;
	glm::mat4 m_transform;

	// Geometry Offset SRT
	glm::vec3 m_geoRotation;
	glm::vec3 m_geoTranslation;
	glm::vec3 m_geoScaling;

	// Not sure what they do yet..
	double m_size;
	double m_limbLength;

	CVS_Bone();
	int initFromFbxNode(FbxNode* _pNode);
	glm::vec3 getGlobalPosition(const glm::mat4* _parentGlobalPosition);
	bool hasBone(const char* _boneName);

	CVS_Bone* getRootBone();
	CVS_Skeleton* getSkeleton();
};
