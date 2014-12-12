#include "CVS_FbxImporter.h"
#include "CVS_Mesh.h"
#include "CVS_Skeleton.h"
#include "CVS_Bone.h"
#include "CVS_Animation.h"

//Standard curve node names
#define FBXSDK_CURVENODE_TRANSLATION	"Translation"
#define FBXSDK_CURVENODE_ROTATION		"Rotation"
#define FBXSDK_CURVENODE_SCALING		"Scaling"
#define FBXSDK_CURVENODE_COMPONENT_X	"X"
#define FBXSDK_CURVENODE_COMPONENT_Y	"Y"
#define FBXSDK_CURVENODE_COMPONENT_Z	"Z"
#define FBXSDK_CURVENODE_COLOR			"C"
#define FBXSDK_CURVENODE_COLOR_RED		FBXSDK_CURVENODE_COMPONENT_X
#define FBXSDK_CURVENODE_COLOR_GREEN	FBXSDK_CURVENODE_COMPONENT_Y
#define FBXSDK_CURVENODE_COLOR_BLUE		FBXSDK_CURVENODE_COMPONENT_Z

#define TRIANGLE_VERTEX_COUNT 3

namespace RTGI
{
	void FbxAMatrixToMat4(const FbxAMatrix* _in, glm::mat4& _out)
	{
		auto m = _in->Double44();
		_out = glm::mat4(
			glm::vec4(m[0][0], m[0][1], m[0][2], m[0][3]),
			glm::vec4(m[1][0], m[1][1], m[1][2], m[1][3]),
			glm::vec4(m[2][0], m[2][1], m[2][2], m[2][3]),
			glm::vec4(m[3][0], m[3][1], m[3][2], m[3][3]));

	}

	CVS_FbxImporter::~CVS_FbxImporter()
	{
		m_pImporter->Destroy();
		m_pIoSettings->Destroy();
		m_pFbxManager->Destroy();
	}

	CVS_FbxImporter::CVS_FbxImporter()
	{
		this->AddFormatSupport(".fbx");
	}

	bool CVS_FbxImporter::init()
	{
		m_pFbxManager = FbxManager::Create();
		m_pIoSettings = FbxIOSettings::Create(m_pFbxManager, IOSROOT);
		m_pFbxManager->SetIOSettings(m_pIoSettings);
		m_pImporter = FbxImporter::Create(m_pFbxManager, "");

		return true;
	}

	bool CVS_FbxImporter::import(const char* _filePath)
	{
		char fileName[20];
		_splitpath_s(_filePath, NULL, 0, NULL, 0, fileName, 20, NULL, 0);
		m_fileName = fileName;

		auto pFbxScene = _initFBXScene(_filePath);
		if (!pFbxScene)
		{
			return false;
		}
		return extractResource(pFbxScene);
	}

	FbxScene* CVS_FbxImporter::_initFBXScene(const char* _filepath)
	{
		// Create the importer.
		int fileFormat = -1;
		if (!m_pFbxManager->GetIOPluginRegistry()->DetectReaderFileFormat(_filepath, fileFormat))
		{
			// Unrecognizable file format. Try to fall back to FbxImporter::eFBX_BINARY
			fileFormat = m_pFbxManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}

		if (!m_pImporter->Initialize(_filepath, -1, m_pFbxManager->GetIOSettings()))
		{
			printf("Call to FbxImporter::Initialize() failed.\n");
			printf("Error returned: %s\n\n", m_pImporter->GetStatus().GetErrorString());
			exit(-1);
		}

		auto pScene = FbxScene::Create(m_pFbxManager, "My scene");
		if (!pScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}

		if (m_pImporter->Import(pScene) == true)
		{
			auto pGlobalSettings = &pScene->GetGlobalSettings();

			// Convert Axis System to what is used in this example, if needed
			FbxAxisSystem SceneAxisSystem = pScene->GetGlobalSettings().GetAxisSystem();
			FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
			if (SceneAxisSystem != OurAxisSystem)
			{
				OurAxisSystem.ConvertScene(pScene);
			}

			// Requires the complete fbxsdk library
			// Convert Unit System to what is used in this example, if needed
			FbxSystemUnit SceneSystemUnit = pScene->GetGlobalSettings().GetSystemUnit();
			if (SceneSystemUnit.GetScaleFactor() != 1.0)
			{
				//The unit in this example is centimeter.
				FbxSystemUnit::cm.ConvertScene(pScene);
			}

			printf("_loadFBXScene: Animation Time Mode : %s\n", FbxGetTimeModeName(pGlobalSettings->GetTimeMode()));

			// Animation Stack
			for (int i = 0; i < pScene->GetSrcObjectCount<FbxAnimStack>(); i++)
			{
				FbxAnimStack* pAnimStack = pScene->GetSrcObject<FbxAnimStack>(i);
				int numAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();

				printf("Animation Stack Name: %s\n\n", pAnimStack->GetName());
				printf("Animation stack contains %d Animation Layer(s)\n", numAnimLayers);

				// Animation Layer
				for (int j = 0; j < numAnimLayers; j++)
				{
					FbxAnimLayer* pAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
					printf("AnimLayer %d\n", j);
					printf("     Node Name: %s\n\n", pScene->GetRootNode()->GetName());
				}
			}


			FbxArray<FbxString*> mAnimStackNameArray;
			// Get the list of all the animation stack.
			pScene->FillAnimStackNameArray(mAnimStackNameArray);

			// Convert mesh, NURBS and patch into triangle mesh
			FbxGeometryConverter lGeomConverter(m_pFbxManager);
			lGeomConverter.Triangulate(pScene, true);

			// Split meshes per material, so that we only have one material per mesh (for VBO support)
			lGeomConverter.SplitMeshesPerMaterial(pScene, true);
		}
		else
		{
			printf("Unable to import file %s.\n", _filepath);
			printf("Error reported: ", m_pImporter->GetStatus().GetErrorString());
		}

		return pScene;
	}

	// Main
	CVS_FbxImporter::FbxNodeOffsetStruct CVS_FbxImporter::getNodeOffset(FbxNode* _pNode)
	{
		FbxNodeOffsetStruct o;
		FbxAMatrixToMat4(&(_pNode->GetAnimationEvaluator()->GetNodeGlobalTransform(_pNode)), o.matGlobalTransform);
		//o.vecPreRotation = cquat(cvec3(glm::radians(glm::make_vec3(_pNode->PreRotation.Get().mData))));
		o.vecLclTranslation = glm::vec3(glm::make_vec3(_pNode->LclTranslation.Get().mData));
		o.vecLclScaling = glm::vec3(glm::make_vec3(_pNode->LclScaling.Get().mData));
		o.vecLclRotation = glm::quat(glm::vec3(glm::radians(glm::make_vec3(_pNode->LclRotation.Get().mData)))) *
			glm::quat(glm::vec3(glm::radians(glm::make_vec3(_pNode->PreRotation.Get().mData))));

		o.matGlobalTransform = o.matGlobalTransform;
		o.matLclTranslation = glm::translate(glm::mat4(), o.vecLclTranslation);
		o.matLclScaling = glm::scale(glm::mat4(), o.vecLclScaling);
		o.matLclRotation = glm::mat4_cast(o.vecLclRotation);

		o.vecGeoTranslation = glm::vec3(glm::make_vec3(_pNode->GetGeometricTranslation(FbxNode::eSourcePivot).mData));
		o.vecGeoRotation = glm::vec3(glm::make_vec3(_pNode->GetGeometricRotation(FbxNode::eSourcePivot).mData));
		o.vecGeoScaling = glm::vec3(glm::make_vec3(_pNode->GetGeometricScaling(FbxNode::eSourcePivot).mData));

		return o;
	}

	bool CVS_FbxImporter::extractResource(FbxScene* _pFbxScene)
	{
		// THE ORDER IS VERY IMPORTANT

		if (hasAnyMesh(_pFbxScene))
		{
			// import skeleton
			ImportSkeletons(_pFbxScene);
			// import mesh
			ImportMeshes(_pFbxScene);
		}
		else
		{
			// import animation
			ImportAnimations(_pFbxScene);
		}

		return true;
	}

	bool CVS_FbxImporter::hasAnyMesh(FbxScene* _pFbxScene)
	{
		auto pTempNode = _pFbxScene->GetRootNode();
		std::queue<FbxNode*> queue;
		queue.push(pTempNode);

		while (pTempNode)
		{
			// do stuff
			auto pNodeAttribute = pTempNode->GetNodeAttribute();
			if (pNodeAttribute)
			{
				if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					return true;
				}
			}

			// remove self
			queue.pop();

			// add child tasks
			for (int i = 0, e = pTempNode->GetChildCount(); i < e; ++i)
			{
				queue.push(pTempNode->GetChild(i));
			}

			pTempNode = queue.empty() ? nullptr : queue.front();
		}

		return false;
	}

	// Skeleton
	void CVS_FbxImporter::ImportSkeletons(FbxScene* _pScene)
	{
		std::queue<FbxNode*> taskQueue;
		std::queue<short> indexQueue;

		auto pRootNode = _pScene->GetRootNode();
		// Ignore scene root
		for (int i = 0, e = pRootNode->GetChildCount(); i < e; ++i)
		{
			taskQueue.push(pRootNode->GetChild(i));
		}
		auto pTempNode = taskQueue.front();
		while (pTempNode)
		{
			if (isBone(pTempNode))
			{
				auto pSkeleton = new CVS_Skeleton;
				m_skeletons.push_back(pSkeleton);
				// size of vector = the end of vector
				auto oldSize = 0, newSize = 0;
				int currentIndex = 0;
				while (pTempNode)
				{
					// do stuff
					if (isBone(pTempNode))
					{
						m_allFbxBoneIndices[pTempNode] = currentIndex;
						auto pParent = pTempNode->GetParent();
						pSkeleton->m_bones.push_back(CVS_Bone());
						pSkeleton->m_bones.back().m_parentIndex = (m_allFbxBoneIndices.find(pParent) == m_allFbxBoneIndices.end()) ? 0xFF : m_allFbxBoneIndices[pParent];
						InitBoneFromFbxNode(&pSkeleton->m_bones[currentIndex], pTempNode);
						++currentIndex;
					}

					// remove self
					taskQueue.pop();

					// fill child tasks
					for (int i = 0, e = pTempNode->GetChildCount(); i < e; ++i)
					{
						taskQueue.push(pTempNode->GetChild(i));
					}

					pTempNode = taskQueue.empty() ? nullptr : taskQueue.front();

				};
			}
			else
			{
				// remove self
				taskQueue.pop();

				pTempNode = taskQueue.empty() ? nullptr : taskQueue.front();
			}

		}

		for (auto i : m_skeletons)
		{
			i->Init();
			m_pResourcePool->push_back(i);
		}
	}

	bool CVS_FbxImporter::isBone(FbxNode* _pNode)
	{
		auto pNodeAttribute = _pNode->GetNodeAttribute();
		if (pNodeAttribute)
		{
			// Blender specifies that Null and Skeleton are both parsed as bones
			if (pNodeAttribute->GetAttributeType() != FbxNodeAttribute::eNull &&
				pNodeAttribute->GetAttributeType() != FbxNodeAttribute::eSkeleton)
			{
				return false;
			}
		}

		return true;
	}

	bool CVS_FbxImporter::InitBoneFromFbxNode(CVS_Bone* _pBone, FbxNode* _pNode)
	{
		// Check what kind of bone this is
		// We assume Empty nodes and nodes containing FbxNulls and FbxSkeletons
		// *Are all bones*
		_pBone->m_name = _pNode->GetName();
		_pBone->m_type = CVS_InternalResource::EType::Bone;

		// todo: pack em in a struct?
		auto offset = getNodeOffset(_pNode);
		_pBone->m_translation = offset.vecLclTranslation;
		_pBone->m_rotation = offset.vecLclRotation;
		_pBone->m_scaling = offset.vecLclScaling;
		_pBone->m_geoTranslation = offset.vecGeoTranslation;
		_pBone->m_geoRotation = offset.vecGeoRotation;
		_pBone->m_geoScaling = offset.vecGeoScaling;

		auto attribute = _pNode->GetNodeAttribute();
		if (attribute)
		{
			auto attributeType = attribute->GetAttributeType();
			switch (attributeType)
			{
			case FbxNodeAttribute::eNull:
			{
			}
				break;
			case FbxNodeAttribute::eSkeleton:
			{

												static const char* skeletonTypes[] = { "Root", "Limb", "Limb Node", "Effector" };

												int childCount = _pNode->GetChildCount();

												FbxSkeleton* pSkeleton = (FbxSkeleton*)_pNode->GetNodeAttribute();

												printf("Bone: %s\n", _pNode->GetName());
												printf("    Type:  %s\n", skeletonTypes[pSkeleton->GetSkeletonType()]);

												if (pSkeleton->GetSkeletonType() == FbxSkeleton::eLimb)
												{
													// TODO: NOT SURE WHAT eLimb DOES
													printf("    Limb Length: %d\n", pSkeleton->LimbLength.Get());
												}
												else if (pSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode)
												{
													printf("    Limb Node Size: %d\n", pSkeleton->Size.Get());
												}
												else if (pSkeleton->GetSkeletonType() == FbxSkeleton::eRoot)
												{
													// TODO: NOT SURE WHAT eRoot DOES
													printf("    Limb Root Size: %d\n", pSkeleton->Size.Get());
												}

												auto boneColor = pSkeleton->GetLimbNodeColor();
												printf("    Color:  %f(red),   %f(green),   %f(blue)\n", boneColor.mRed, boneColor.mGreen, boneColor.mBlue);

												// Fill data
												_pBone->m_color = glm::vec3(boneColor.mRed, boneColor.mGreen, boneColor.mBlue);
												_pBone->m_size = pSkeleton->Size.Get();
												_pBone->m_limbLength = pSkeleton->LimbLength.Get();

			}
			}
		}
		return true;
	}

	// Meshes
	void CVS_FbxImporter::ImportMeshes(FbxScene* _pScene)
	{
		std::queue<FbxNode*> taskQueue;

		auto pRootNode = _pScene->GetRootNode();
		// Ignore scene root
		for (int i = 0, e = pRootNode->GetChildCount(); i < e; ++i)
		{
			taskQueue.push(pRootNode->GetChild(i));
		}
		auto pTempNode = taskQueue.front();

		while (pTempNode)
		{
			if (isMesh(pTempNode))
				// 			for (int k = 0; k < pTempNode->GetNodeAttributeCount(); k++)
				// 			{
				// 				auto pSubMesh = (FbxMesh*)pTempNode->GetNodeAttributeByIndex(k);

				// do stuff
			{
				auto pMesh = new CVS_Mesh();
				m_meshes.push_back(pMesh);
				InitMeshFromFbxNode(pMesh, pTempNode);
				// TODO: possibly allow custom skeleton
				//pMesh->m_pSkeleton = m_skeletons[0];
				//InitMeshSkeletonWithCurrentFbxScene(pMesh, pTempNode);

			}
			// fill child tasks
			for (int i = 0, e = pTempNode->GetChildCount(); i < e; ++i)
			{
				taskQueue.push(pTempNode->GetChild(i));
			}
			// remove self
			taskQueue.pop();
			pTempNode = taskQueue.empty() ? nullptr : taskQueue.front();
		};

		for (auto i : m_meshes)
		{
			i->Init();
			m_pResourcePool->push_back(i);
		}
		//		}
	}

	bool CVS_FbxImporter::isMesh(FbxNode* _pNode)
	{
		auto pNodeAttribute = _pNode->GetNodeAttribute();
		if (pNodeAttribute)
		{
			if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				return true;
			}
		}

		return false;
	}

	bool CVS_FbxImporter::InitMeshFromFbxNode(CVS_Mesh* _pMesh, FbxNode* _pNode)
	{
		_pMesh->m_name = _pNode->GetName();

		// Get the mesh offset
		auto offset = getNodeOffset(_pNode);
		_pMesh->m_offset = offset.matLclTranslation * /*offset.matLclRotation **/ offset.matLclScaling * offset.matGlobalTransform;

		// Create Submeshes
		int totalVertexCount = 0;
		int totalTriangleCount = 0;
		for (int iSubmesh = 0; iSubmesh < _pNode->GetNodeAttributeCount(); iSubmesh++)
		{
			auto pFbxSubMesh = (FbxMesh*)_pNode->GetNodeAttributeByIndex(iSubmesh);
			if (!pFbxSubMesh)
			{
				printf("Mesh::initFromFbxNode: Not a valid mesh node. %s\n", _pNode->GetName());
				return 1;
			}
			else
			{
				printf("Mesh::initFromFbxNode: Mesh processed: %s\n", _pNode->GetName());
			}

			auto lPolyCount = pFbxSubMesh->GetPolygonCount();
			int lVertexCount = pFbxSubMesh->GetControlPointsCount();
			auto lMaterialIndices = &pFbxSubMesh->GetElementMaterial()->GetIndexArray();
			const int lMaterialIndex = lMaterialIndices->GetAt(0);

			auto lMaterialMappingMode = FbxGeometryElement::eNone;

			// Create new submesh
			if (pFbxSubMesh->GetElementMaterial())
			{
				lMaterialMappingMode = pFbxSubMesh->GetElementMaterial()->GetMappingMode();
				if (lMaterialIndices && lMaterialMappingMode == FbxGeometryElement::eAllSame)
				{
					FBX_ASSERT(lMaterialIndices->GetCount() == lPolyCount);
					auto matIndexCount = lMaterialIndices->GetCount();
					//*
					// Count the faces of each material

					if (_pMesh->m_subMeshes.size() < lMaterialIndex + 1)
					{
						_pMesh->m_subMeshes.resize(lMaterialIndex + 1);
					}
					if (_pMesh->m_subMeshes[lMaterialIndex] == NULL)
					{
						_pMesh->m_subMeshes[lMaterialIndex] = new CVS_SubMesh;
					}
					_pMesh->m_subMeshes[lMaterialIndex]->m_triangleCount = lPolyCount;
					_pMesh->m_subMeshes[lMaterialIndex]->m_indexOffset = totalTriangleCount * TRIANGLE_VERTEX_COUNT;
					totalVertexCount += lVertexCount;
					totalTriangleCount += lPolyCount;
					//*/
				}
			}

			// Initialize submesh
			auto pSubMesh = _pMesh->m_subMeshes[lMaterialIndex];
			// Congregate all the data of a mesh to be cached in VBOs.
			// If normal or UV is by polygon vertex, record all vertex attributes by polygon vertex.
			bool hasNormal = pFbxSubMesh->GetElementNormalCount() > 0;
			bool hasUV = pFbxSubMesh->GetElementUVCount() > 0;
			bool bAllByControlPoint = true;
			auto lNormalMappingMode = FbxGeometryElement::eNone;
			auto lUVMappingMode = FbxGeometryElement::eNone;

			_pMesh->mVertexData.resize(totalVertexCount);
			_pMesh->mVertexNormalData.resize(totalVertexCount);
			_pMesh->mTCoordData.resize(totalVertexCount);
			_pMesh->mIndexData.resize(totalTriangleCount * TRIANGLE_VERTEX_COUNT);
			FbxStringList lUVNames;
			pFbxSubMesh->GetUVSetNames(lUVNames);
			const char * lUVName = NULL;
			if (hasUV && lUVNames.GetCount())
			{
				lUVName = lUVNames[0];
			}

			const FbxVector4 * lControlPoints = pFbxSubMesh->GetControlPoints();

			glm::vec3 lCurrentVertex;
			glm::vec3 lCurrentNormal;
			glm::vec2 lCurrentUV;
			const FbxGeometryElementNormal * lNormalElement = NULL;
			const FbxGeometryElementUV * lUVElement = NULL;
			for (int iTarget = totalVertexCount - lVertexCount; iTarget < totalVertexCount; ++iTarget)
			{
				int iSource = iTarget - (totalVertexCount - lVertexCount);
				// Save the vertex position.
				lCurrentVertex = glm::vec3(
					lControlPoints[iSource][0],
					lControlPoints[iSource][1],
					lControlPoints[iSource][2]);

				_pMesh->mVertexData[iTarget] = lCurrentVertex;

				// Save the normal.
				if (hasNormal)
				{
					lNormalElement = pFbxSubMesh->GetElementNormal(0);

					int lNormalIndex = iSource;
					lCurrentNormal = glm::vec3(
						lNormalElement->GetDirectArray().GetAt(lNormalIndex)[0],
						lNormalElement->GetDirectArray().GetAt(lNormalIndex)[1],
						lNormalElement->GetDirectArray().GetAt(lNormalIndex)[2]);
					_pMesh->mVertexNormalData[iTarget] = -lCurrentNormal;
				}

				// Save the UV.
				if (hasUV)
				{
					lUVElement = pFbxSubMesh->GetElementUV(0);

					int lUVIndex = iSource;
					lCurrentUV = glm::vec2(
						lUVElement->GetDirectArray().GetAt(lUVIndex)[0],
						lUVElement->GetDirectArray().GetAt(lUVIndex)[1]);
					_pMesh->mTCoordData[iTarget] = lCurrentUV;

				}
			}

			// Get indices
			for (int polyIndex = totalTriangleCount - lPolyCount; polyIndex < totalTriangleCount; ++polyIndex)
			{
				for (int j = 0; j < TRIANGLE_VERTEX_COUNT; ++j)
				{
					const int lControlPointIndex = pFbxSubMesh->GetPolygonVertex(polyIndex - (totalTriangleCount - lPolyCount), j);
					_pMesh->mIndexData[polyIndex * TRIANGLE_VERTEX_COUNT + j] = static_cast<unsigned int>((totalVertexCount - lVertexCount) + lControlPointIndex);
					
					
// 					FbxVector4 lNormal;
// 					pFbxSubMesh->GetPolygonVertexNormal(polyIndex - (totalTriangleCount - lPolyCount), j, lNormal);
// 					lCurrentNormal = glm::vec3(
// 						lNormal[0],
// 						lNormal[1],
// 						lNormal[2]);
// 					_pMesh->mVertexNormalData[lVertexCount] = lCurrentNormal;
				}
			}
		}
		auto pSubMesh = _pNode->GetMesh();

#pragma region
		// 		if (hasNormal)
		// 		{
		// 			lNormalMappingMode = pSubMesh->GetElementNormal(0)->GetMappingMode();
		// 			if (lNormalMappingMode == FbxGeometryElement::eNone)
		// 			{
		// 				hasNormal = false;
		// 			}
		// 			// TODO: Really should be hasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint
		// 			// Here, but the API tell us the geometry layout the exact opposite.
		// 			// If it contains split normals, it tells you it's by control point, and vice versa
		// 			// DON'T TRUST ANYONE
		// 			if (hasNormal && lNormalMappingMode != FbxGeometryElement::eByControlPoint)
		// 			{
		// 				// It seems to be bugged. We'll treat everything as bycontrolpoint
		// 				bAllByControlPoint = true;
		// 			}
		// 		}
		// 		if (hasUV)
		// 		{
		// 			lUVMappingMode = pSubMesh->GetElementUV(0)->GetMappingMode();
		// 			if (lUVMappingMode == FbxGeometryElement::eNone)
		// 			{
		// 				hasUV = false;
		// 			}
		// 			if (hasUV && lUVMappingMode != FbxGeometryElement::eByControlPoint)
		// 			{
		// 				// It seems to be bugged. We'll treat everything as bycontrolpoint
		// 				bAllByControlPoint = true;
		// 			}
		// 		}

		// Allocate the array memory, by control point or by polygon vertex.



		// Populate the array with vertex attribute, if by control point.

#pragma endregion Inject Data: vertices, normal, UV
		return true;
	}

	bool CVS_FbxImporter::InitMeshSkeletonWithCurrentFbxScene(CVS_Mesh* _pMesh, FbxNode* _pNode)
	{
		auto pFbxMesh = _pNode->GetMesh();
		_pMesh->mBoneData.resize(_pMesh->mVertexData.size());
		int skinCount = pFbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		if (skinCount)
		{
			for (int skinIndex = 0; skinIndex != skinCount; ++skinIndex)
			{
				auto pSkin = (FbxSkin *)pFbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin);
				if (skinCount > 1)
				{
					printf("CVS_Mesh::initFromFbxNode: Mesh contains more than 1 skin.\n");
					assert(0);
				}
				static int what = 0;

				// Allocate memory for vertex bone data
				_pMesh->mBoneData.reserve(_pMesh->mVertexData.size());
				// for each skin, find the correct skeleton
				// TODO: might be buggy when there's nore than 1 skeleton per mesh
				CVS_Skeleton* pSkeleton = nullptr;
				int clusterCount = (pSkin)->GetClusterCount();
				for (int clusterIndex = 0; clusterIndex != clusterCount; ++clusterIndex)
				{
					auto pCluster = ((FbxSkin *)pFbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin))->GetCluster(clusterIndex);
					_PrintClusterInfo(pCluster, clusterIndex);

					// For each cluster, we check against the bone map to make sure
					// everything comes from the same skeleton tree
					auto pBoneNode = pCluster->GetLink();
					auto boneIndex = m_allFbxBoneIndices[pBoneNode];
					auto pBone = &_pMesh->m_pSkeleton->m_bones[boneIndex];

					// get bone inverse bind pose
					FbxAMatrix transformMatrix;
					pCluster->GetTransformMatrix(transformMatrix); // The transformation of the mesh at binding time
					FbxAMatrix transformLinkMatrix;
					pCluster->GetTransformLinkMatrix(transformLinkMatrix); // The transformation of the cluster(joint) at binding time from joint space to world space
					FbxAMatrix globalBindposeInverseMatrix = transformLinkMatrix.Inverse();
					auto pMat = glm::mat4();
					auto pMatLink = glm::mat4();
					auto pBindPoseInverse = glm::mat4();
					FbxAMatrixToMat4(&transformMatrix, pMat);
					FbxAMatrixToMat4(&transformLinkMatrix, pMatLink);
					FbxAMatrixToMat4(&globalBindposeInverseMatrix, pBindPoseInverse);
					pBone->m_bindPoseInv *= pBindPoseInverse * pMat;

					// Inject data into vertices
					int* lIndices = pCluster->GetControlPointIndices();
					double* lWeights = pCluster->GetControlPointWeights();
					for (int j = 0, e = pCluster->GetControlPointIndicesCount(); j < e; j++)
					{

						int index = lIndices[j];
						if (index == 0)
						{
							what++;
						}

						_pMesh->mBoneData[index].AddBoneData(boneIndex, lWeights[j]);
					}
				}
			}

		}
		else
		{
			// TODO: fix shader support for static mesh

			// If there's no bone at all, we make a dummy
			// to pass to shaders
			/*
			if (!pMesh->m_pSkeleton)
			{
			auto pDummyBone = new CVS_Bone;
			pDummyBone->m_name = "DummyBone";
			pDummyBone->m_color = cvec3(1.0f, 1.0f, 1.0f);
			pDummyBone->m_index = MAX_BONES - 1;
			this->m_pSkeleton = pDummyBone;
			// 			for (int i = 0, e = m_vertexBones.size(); i < e; i++)
			// 			{
			// 				m_vertexBones[i].AddBoneData(0, 1.0f);
			// 			}
			}
			//*/
		}
		return true;
	}

	void CVS_FbxImporter::_PrintClusterInfo(FbxCluster* _pCluster, int _index)
	{
		static const char* lClusterModes[] = { "Normalize", "Additive", "Total1" };

		printf("    Cluster %d\n", _index);

		printf("    Mode: %s\n", lClusterModes[_pCluster->GetLinkMode()]);

		if (_pCluster->GetLink() != NULL)
		{
			printf("        Name: %s\n", (char *)_pCluster->GetLink()->GetName());
		}

		FbxString lString1 = "        Link Indices: ";
		FbxString lString2 = "        Weight Values: ";

		int k, lIndexCount = _pCluster->GetControlPointIndicesCount();
		int* lIndices = _pCluster->GetControlPointIndices();
		double* lWeights = _pCluster->GetControlPointWeights();

		for (k = 0; k < lIndexCount; k++)
		{
			lString1 += lIndices[k];
			lString2 += (float)lWeights[k];

			if (k < lIndexCount - 1)
			{
				lString1 += ", ";
				lString2 += ", ";
			}
		}

		lString1 += "\n";
		lString2 += "\n";

		FBXSDK_printf(lString1);
		//FBXSDK_printf(lString2);

		printf("\n");

		FbxAMatrix lMatrix;

		lMatrix = _pCluster->GetTransformMatrix(lMatrix);
		printf("        Transform Translation     : %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
		printf("        Transform Rotation        : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
		printf("        Transform Scaling         : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);

		lMatrix = _pCluster->GetTransformLinkMatrix(lMatrix);
		printf("        Transform Link Translation: %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
		printf("        Transform Link Rotation   : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
		printf("        Transform Link Scaling    : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);

		if (_pCluster->GetAssociateModel() != NULL)
		{
			lMatrix = _pCluster->GetTransformAssociateModelMatrix(lMatrix);
			printf("        Associate Model            : %s\n", (char *)_pCluster->GetAssociateModel()->GetName());
			printf("        Associate Model Translation: %f, %f, %f\n", lMatrix.GetT()[0], lMatrix.GetT()[1], lMatrix.GetT()[2]);
			printf("        Associate Model Rotation   : %f, %f, %f\n", lMatrix.GetR()[0], lMatrix.GetR()[1], lMatrix.GetR()[2]);
			printf("        Associate Model Scaling    : %f, %f, %f\n", lMatrix.GetS()[0], lMatrix.GetS()[1], lMatrix.GetS()[2]);
		}

		printf("\n");
	}

	// Animations
	void CVS_FbxImporter::ImportAnimations(FbxScene* _pScene)
	{
		//GETTING ANIMAION DATA
		char pTimeString[20];
		auto& gSettings = _pScene->GetGlobalSettings();
		auto timeMode = gSettings.GetTimeMode();
		printf("Time Mode : %s\n", FbxGetTimeModeName(timeMode));
		m_globalAnimationFrameRate = FrameRateToDouble(timeMode);
		FbxTimeSpan pTimeSpan;
		gSettings.GetTimelineDefaultTimeSpan(pTimeSpan);
		auto lStart = pTimeSpan.GetStart();
		auto lEnd = pTimeSpan.GetStop();
		printf("Timeline default timespan: \n");
		printf("     Start: %s\n", lStart.GetTimeString(pTimeString, FbxUShort(20)));
		printf("     Stop : %s\n", lEnd.GetTimeString(pTimeString, FbxUShort(20)));

		// Print a brief info of all anim stacks
		for (int iFbxAnimStack = 0, e = _pScene->GetSrcObjectCount<FbxAnimStack>(); iFbxAnimStack < e; ++iFbxAnimStack)
		{
			// Stack Name
			auto pFbxAnimStack = _pScene->GetSrcObject<FbxAnimStack>(iFbxAnimStack);
			auto stackName = pFbxAnimStack->GetName();
			printf("Animation Stack %d Name: %s\n", iFbxAnimStack, stackName);
			// Time
			auto start = pFbxAnimStack->GetLocalTimeSpan().GetStart();
			auto stop = pFbxAnimStack->GetLocalTimeSpan().GetStop();
			auto animationLength = stop.GetFrameCount(timeMode) - start.GetFrameCount(timeMode);// TODO: do we really need +1?;
			printf("     Start: %d\n", start.GetFrameCount(timeMode));
			printf("     Stop : %d\n", stop.GetFrameCount(timeMode));
		}

		InitAnimsInFbxScene(_pScene);
	}

	void CVS_FbxImporter::InitAnimsInFbxScene(FbxScene* _pScene)
	{
		for (int iFbxAnimStack = 0, e = _pScene->GetSrcObjectCount<FbxAnimStack>(); iFbxAnimStack < e; ++iFbxAnimStack)
		{
			auto pFbxAnimStack = _pScene->GetSrcObject<FbxAnimStack>(iFbxAnimStack);
			// Init Animation from stack
			auto pAnimation = new CVS_Animation;
			pAnimation->m_frameRate = m_globalAnimationFrameRate;

			auto& gSettings = _pScene->GetGlobalSettings();
			auto timeMode = gSettings.GetTimeMode();
			auto start = pFbxAnimStack->GetLocalTimeSpan().GetStart();
			auto stop = pFbxAnimStack->GetLocalTimeSpan().GetStop();
			// Time
			pAnimation->m_frameCount = stop.GetFrameCount(timeMode) - start.GetFrameCount(timeMode) + 1;// TODO: why do we need +1?;


			InitAnimFromFbxAnimStack(_pScene->GetRootNode(), pFbxAnimStack, pAnimation);
			m_pResourcePool->push_back(pAnimation);
			char fileNameTag[10];
			// TODO: improve naming
			sprintf_s(fileNameTag, "-Take%d", iFbxAnimStack);
			pAnimation->m_name = m_fileName + fileNameTag;
		}
	}

	void CVS_FbxImporter::InitAnimFromFbxAnimStack(FbxNode* _pNode, FbxAnimStack* _pFbxAnimStack, CVS_Animation* _pAnimation)
	{
		// Fill with animation layer data
		for (int i = 0, e = _pFbxAnimStack->GetMemberCount<FbxAnimLayer>(); i < e; ++i)
		{
			FbxAnimLayer* pFbxAnimLayer = _pFbxAnimStack->GetMember<FbxAnimLayer>(i);
			auto layerName = pFbxAnimLayer->GetName();
			printf("Animation Layer Name: %s\n", layerName);
			auto pAnimationLayer = new CVS_AnimationLayer;

			std::queue<FbxNode*> taskQueue;

			auto pRootNode = _pNode; // _pScene->GetRootNode();
			// Ignore scene root
			for (int i = 0, e = pRootNode->GetChildCount(); i < e; ++i)
			{
				taskQueue.push(pRootNode->GetChild(i));
			}
			auto pTempNode = taskQueue.front();

			while (pTempNode)
			{
				// do stuff
				if (isBone(pTempNode))
				{
					pAnimationLayer->m_animNodes.push_back(CVS_AnimationNode());
					auto pAnimationNode = &pAnimationLayer->m_animNodes.back();
					InitAnimNodeFromFbxNode(pAnimationNode, pTempNode, pFbxAnimLayer, pAnimationLayer);
				}
				// remove self
				taskQueue.pop();
				// fill child tasks
				for (int i = 0, e = pTempNode->GetChildCount(); i < e; ++i)
				{
					taskQueue.push(pTempNode->GetChild(i));
				}
				pTempNode = taskQueue.empty() ? nullptr : taskQueue.front();
			};





			// Fill attributes and push back
			pAnimationLayer->m_name = layerName;
			pAnimationLayer->m_weight = pFbxAnimLayer->Weight;
			_pAnimation->m_animLayers.push_back(pAnimationLayer);
		}
	}

	void CVS_FbxImporter::InitAnimNodeFromFbxAnimLayer(FbxNode* _pNode, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationLayer* _pAnimLayer)
	{

	}

	void CVS_FbxImporter::InitAnimNodesFromFbxAnimLayerRecursive(FbxNode* _pNode, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationLayer* _pAnimLayer)
	{
		auto pAnimationNode = new CVS_AnimationNode;
		pAnimationNode->m_name = _pNode->GetName();
		if (InitAnimNodeFromFbxNode(pAnimationNode, _pNode, _pFbxAnimLayer, _pAnimLayer))
		{
			// Add bone to all bone list
			auto& allNodesInCurrentLayer = m_layerNodeMaps[_pFbxAnimLayer];
			allNodesInCurrentLayer[_pNode] = pAnimationNode;
			// Resolving the bone tree relies on correctly traversing the scene nodes
			// Try to find its parent
			auto pParentFbxNode = _pNode->GetParent();
			if (pParentFbxNode)
			{
				auto pParentAnimNode = static_cast<CVS_AnimationNode*>(allNodesInCurrentLayer[pParentFbxNode]);
				if (!pParentAnimNode)
				{
					// WTF??
					printf("NOT POSSIBLE.\n");
					assert(0);
				}
				else
				{
					// Create Link between parent and our new bone
					pParentAnimNode->m_children.push_back(pAnimationNode);
					pAnimationNode->m_pParent = pParentAnimNode;
				}
			}
			else
			{
				if (_pAnimLayer->m_pRootAnimNode)
				{
					printf("err multiple root nodes for anim layer.\n");
					assert(0);
				}
				_pAnimLayer->m_pRootAnimNode = pAnimationNode;
			}
		}

		// Process children
		for (int i = 0, e = _pNode->GetChildCount(); i < e; ++i)
		{
			InitAnimNodesFromFbxAnimLayerRecursive(_pNode->GetChild(i), _pFbxAnimLayer, _pAnimLayer);
		}
	}

	bool CVS_FbxImporter::InitAnimNodeFromFbxNode(CVS_AnimationNode* _pAnimNode, FbxNode* _pFbxNode, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationLayer* _pAnimLayer)
	{
		_pAnimNode->m_name = _pFbxNode->GetName();

		// For each node, extract all channels
		static const char* AnimCurveTypes[] = { FBXSDK_CURVENODE_TRANSLATION, FBXSDK_CURVENODE_ROTATION, FBXSDK_CURVENODE_SCALING };
		static const char* AnimComponents[] = { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };
		FbxProperty pProperties[CVS_AnimationNode::ECurveType::NumCurveTypes] = {
			_pFbxNode->LclTranslation,
			_pFbxNode->LclRotation,
			_pFbxNode->LclScaling
		};

		auto pTCurve = pProperties[0].GetCurve(_pFbxAnimLayer);
		auto pRCurve = pProperties[1].GetCurve(_pFbxAnimLayer);
		auto pSCurve = pProperties[2].GetCurve(_pFbxAnimLayer);

		int curveKeyCount[3] = {
			pTCurve ? pTCurve->KeyGetCount() : 0,
			pRCurve ? pRCurve->KeyGetCount() : 0,
			pSCurve ? pRCurve->KeyGetCount() : 0
		};

		bool hasAnyCurve = curveKeyCount[0] + curveKeyCount[1] + curveKeyCount[2];

		if (hasAnyCurve)
		{
			// Loop through all curves
			for (int iCurveType = 0; iCurveType < CVS_AnimationNode::ECurveType::NumCurveTypes; iCurveType++)
			{
				if (curveKeyCount[iCurveType])
				{
					auto pAnimCurve = new CVS_AnimationCurve;
					pAnimCurve->m_name = _pFbxNode->GetName();
					_pAnimNode->m_channels[iCurveType] = pAnimCurve;
					printf("        %s: %s\n", pAnimCurve->m_name.c_str(), AnimCurveTypes[iCurveType]);
					InitAnimCurveFromFbxProperty(&pProperties[iCurveType], _pFbxAnimLayer, pAnimCurve);

					if (iCurveType == CVS_AnimationNode::ECurveType::Rotation)
					{
						auto q = glm::quat(glm::radians(pAnimCurve->m_keyFrames[0].m_keyValue));
						auto offset = getNodeOffset(_pFbxNode);
						auto translation = offset.vecLclTranslation;
						auto rotation = offset.vecLclRotation;
						auto scaling = offset.vecLclScaling;
						auto geoTranslation = offset.vecGeoTranslation;
						auto geoRotation = offset.vecGeoRotation;
						auto geoScaling = offset.vecGeoScaling;


						auto newq = rotation * q;
						auto vq = glm::degrees(glm::eulerAngles(q));
						int qwfqwf = 0;
					}

				}
			}
		}
		return true;
	}

	void CVS_FbxImporter::InitAnimCurveFromFbxProperty(FbxProperty* _pFbxProperty, FbxAnimLayer* _pFbxAnimLayer, CVS_AnimationCurve* _pAnimCurve)
	{

		static const char* AnimComponents[] = { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };
		char    keyTimeString[24];
		FbxAnimCurve* pCurves[3] = {
			_pFbxProperty->GetCurve(_pFbxAnimLayer, AnimComponents[0]),
			_pFbxProperty->GetCurve(_pFbxAnimLayer, AnimComponents[1]),
			_pFbxProperty->GetCurve(_pFbxAnimLayer, AnimComponents[2]) };

		CVS_AnimationKeyFrame tempFrame;
		_pAnimCurve->m_keyFrames.reserve(pCurves[0]->KeyGetCount());
		for (int iKey = 0, keyCount = pCurves[0]->KeyGetCount(); iKey < keyCount; iKey++)
		{
			auto keyTime = pCurves[0]->KeyGetTime(iKey);
			keyTime.GetTimeString(keyTimeString, FbxUShort(24));
			printf("Key: %s    ", keyTimeString);
			for (int iComponent = 0; iComponent < 3; ++iComponent)
			{
				auto keyValue = static_cast<float>(pCurves[iComponent]->KeyGetValue(iKey));
				// Fill data for key frame
				tempFrame.m_interpolationType = static_cast<CVS_AnimationKeyFrame::EInterpolationType>(pCurves[iComponent]->KeyGetInterpolation(iKey));
				tempFrame.m_keyValue[iComponent] = keyValue;
				printf("%s %.2f  ", AnimComponents[iComponent], keyValue);
			}
			printf("\n");
			_pAnimCurve->m_keyFrames.push_back(tempFrame);
		}
	}

	void CVS_FbxImporter::_DisplayCurveKeys(FbxAnimCurve* pCurve)
	{
		static const char* interpolation[] = { "?", "constant", "linear", "cubic" };
		static const char* constantMode[] = { "?", "Standard", "Next" };
		static const char* cubicMode[] = { "?", "Auto", "Auto break", "Tcb", "User", "Break", "User break" };
		static const char* tangentWVMode[] = { "?", "None", "Right", "Next left" };

		FbxTime   lKeyTime;
		float   lKeyValue;
		char    lTimeString[256];
		FbxString lOutputString;
		int     lCount;

		int lKeyCount = pCurve->KeyGetCount();

		for (lCount = 0; lCount < lKeyCount; lCount++)
		{
			auto key = pCurve->KeyGet(lCount);

			lKeyValue = static_cast<float>(pCurve->KeyGetValue(lCount));
			lKeyTime = pCurve->KeyGetTime(lCount);

			lOutputString = "            Key Time: ";
			lOutputString += lKeyTime.GetTimeString(lTimeString, FbxUShort(256));
			lOutputString += ".... Key Value: ";
			lOutputString += lKeyValue;
			lOutputString += " [ ";
			lOutputString += interpolation[InterpolationFlagToIndex(pCurve->KeyGetInterpolation(lCount))];
			if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant)
			{
				lOutputString += " | ";
				lOutputString += constantMode[ConstantmodeFlagToIndex(pCurve->KeyGetConstantMode(lCount))];
			}
			else if ((pCurve->KeyGetInterpolation(lCount)&FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic)
			{
				lOutputString += " | ";
				lOutputString += cubicMode[TangentmodeFlagToIndex(pCurve->KeyGetTangentMode(lCount))];
				lOutputString += " | ";
				lOutputString += tangentWVMode[TangentweightFlagToIndex(pCurve->KeyGet(lCount).GetTangentWeightMode())];
				lOutputString += " | ";
				lOutputString += tangentWVMode[TangentVelocityFlagToIndex(pCurve->KeyGet(lCount).GetTangentVelocityMode())];
			}
			lOutputString += " ]";
			lOutputString += "\n";
			FBXSDK_printf(lOutputString);
		}
	}

	int CVS_FbxImporter::TangentVelocityFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eVelocityNone) == FbxAnimCurveDef::eVelocityNone) return 1;
		if ((flags & FbxAnimCurveDef::eVelocityRight) == FbxAnimCurveDef::eVelocityRight) return 2;
		if ((flags & FbxAnimCurveDef::eVelocityNextLeft) == FbxAnimCurveDef::eVelocityNextLeft) return 3;
		return 0;
	}

	int CVS_FbxImporter::TangentweightFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eWeightedNone) == FbxAnimCurveDef::eWeightedNone) return 1;
		if ((flags & FbxAnimCurveDef::eWeightedRight) == FbxAnimCurveDef::eWeightedRight) return 2;
		if ((flags & FbxAnimCurveDef::eWeightedNextLeft) == FbxAnimCurveDef::eWeightedNextLeft) return 3;
		return 0;
	}

	int CVS_FbxImporter::TangentmodeFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eTangentAuto) == FbxAnimCurveDef::eTangentAuto) return 1;
		if ((flags & FbxAnimCurveDef::eTangentAutoBreak) == FbxAnimCurveDef::eTangentAutoBreak) return 2;
		if ((flags & FbxAnimCurveDef::eTangentTCB) == FbxAnimCurveDef::eTangentTCB) return 3;
		if ((flags & FbxAnimCurveDef::eTangentUser) == FbxAnimCurveDef::eTangentUser) return 4;
		if ((flags & FbxAnimCurveDef::eTangentGenericBreak) == FbxAnimCurveDef::eTangentGenericBreak) return 5;
		if ((flags & FbxAnimCurveDef::eTangentBreak) == FbxAnimCurveDef::eTangentBreak) return 6;
		return 0;
	}

	int CVS_FbxImporter::ConstantmodeFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eConstantStandard) == FbxAnimCurveDef::eConstantStandard) return 1;
		if ((flags & FbxAnimCurveDef::eConstantNext) == FbxAnimCurveDef::eConstantNext) return 2;
		return 0;
	}

	int CVS_FbxImporter::InterpolationFlagToIndex(int flags)
	{
		if ((flags & FbxAnimCurveDef::eInterpolationConstant) == FbxAnimCurveDef::eInterpolationConstant) return 1;
		if ((flags & FbxAnimCurveDef::eInterpolationLinear) == FbxAnimCurveDef::eInterpolationLinear) return 2;
		if ((flags & FbxAnimCurveDef::eInterpolationCubic) == FbxAnimCurveDef::eInterpolationCubic) return 3;
		return 0;
	}

	double CVS_FbxImporter::FrameRateToDouble(FbxTime::EMode fp, double customFPSVal /*= -1.0*/)
	{
		switch (fp)
		{
		case FbxTime::EMode::eDefaultMode:
			return 1.0;

		case FbxTime::EMode::eFrames120:
			return 120.0;

		case FbxTime::EMode::eFrames100:
			return 100.0;

		case FbxTime::EMode::eFrames60:
			return 60.0;

		case FbxTime::EMode::eFrames50:
			return 50.0;

		case FbxTime::EMode::eFrames48:
			return 48.0;

		case FbxTime::EMode::eFrames30:
		case FbxTime::EMode::eFrames30Drop:
			return 30.0;

		case FbxTime::EMode::eNTSCDropFrame:
		case FbxTime::EMode::eNTSCFullFrame:
			return 29.9700262;

		case FbxTime::EMode::ePAL:
			return 25.0;

		case FbxTime::EMode::eFrames24:
			return 24.0;

		case FbxTime::EMode::eFrames1000:
			return 1000.0;

		case FbxTime::EMode::eFilmFullFrame:
			return 23.976;

		case FbxTime::EMode::eCustom:
			return customFPSVal;

		case FbxTime::EMode::eModesCount:
			break;
		}

		assert(0);
		return -1.0f; // this is to silence compiler warnings
	}
}