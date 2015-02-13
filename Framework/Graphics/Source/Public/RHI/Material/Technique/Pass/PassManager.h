//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PassManager_H
#define RTGI_PassManager_H

#include "RefObject.h"
#include "PassBase.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/23/2014
//----------------------------------------------------------------------------
class PassManager : public RefObject
{
public:
    PassManager();
    ~PassManager();

    void AddPass(PassBase* pass);
    unsigned int GetPassCount() const;
    PassBase* GetPass(unsigned int i) const;

    void CreateDeviceResource(GPUDevice* device);

protected:
    std::vector<PassBase*> mPasses;
};

typedef RefPointer<PassManager> PassManagerPtr;

}

#endif