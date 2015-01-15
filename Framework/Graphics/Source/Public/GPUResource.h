//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GPUResource_H
#define RTGI_GPUResource_H


#include "RefObject.h"

namespace RTGI
{

class GPUDevice;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class GPUResource : public RefObject
{
public:
    GPUResource();
    virtual ~GPUResource();

    GPUDevice* Device;
};

typedef RefPointer<GPUResource> GPUResourcePtr;

struct ShaderHandle : public GPUResource
{
};

struct ShaderProgramHandle : public GPUResource
{
};

struct PassInfoHandle : public GPUResource
{
};

struct ShaderUniformHandle : public GPUResource
{
};

struct TextureHandle : public GPUResource
{
};

}

#endif