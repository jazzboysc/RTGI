#pragma once
#include "CVS_SubSystem.h"
#include "CVS_Resource.h"

#include "CVS_Skeleton.h"
#include "CVS_Animation.h"

namespace RTGI
{
	class CVS_ResourceImporter;
	struct CVS_ResourceSystem : public CVS_SubSystem, public RefObject
	{

	public:
		virtual bool Initialize();

		virtual bool End();

		bool import(const char* _filePath);
		CVS_Resource* get(const char* _resourceName, CVS_Resource::EType _type);

		// Data types
		std::vector<CVS_ResourceImporter*> m_importers;
		std::vector<CVS_Resource*> m_resources;

		// Helper functions
		//FbxScene* _initFBXScene(char* filepath);
		//CVS_Scene* loadFileToScene(char* filename, CVS_Scene* _pScene);

	};

	typedef RefPointer<CVS_ResourceSystem> ResourceSystemPtr;

}