//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "PassManager.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PassManager::PassManager()
{
}
//----------------------------------------------------------------------------
PassManager::~PassManager()
{
    for( int i = 0; i < (int)mPasses.size(); ++i )
    {
        delete mPasses[i];
    }
}
//----------------------------------------------------------------------------
void PassManager::AddPass(PassBase* pass)
{
    RTGI_ASSERT(pass);
    mPasses.push_back(pass);
}
//----------------------------------------------------------------------------
unsigned int PassManager::GetPassCount() const
{
    return (unsigned int)mPasses.size();
}
//----------------------------------------------------------------------------
PassBase* PassManager::GetPass(unsigned int i) const
{
    return mPasses[i];
}
//----------------------------------------------------------------------------
void PassManager::CreateDeviceResource(GPUDevice* device)
{
    for( int i = 0; i < (int)mPasses.size(); ++i )
    {
        mPasses[i]->CreateDeviceResource(device);
    }
}
//----------------------------------------------------------------------------