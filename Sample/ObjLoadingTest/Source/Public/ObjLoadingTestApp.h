#ifndef RTGI_ObjLoadingTestApp_H
#define RTGI_ObjLoadingTestApp_H

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/13/2015
//----------------------------------------------------------------------------
class ObjLoadingTestApp : public Application
{
public:
    ObjLoadingTestApp(int width = 1024, int height = 768);
    ~ObjLoadingTestApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

};

}

#endif