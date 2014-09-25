#include "VPLSampleRSM.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLSampleRSM::VPLSampleRSM()
{
}
//----------------------------------------------------------------------------
VPLSampleRSM::~VPLSampleRSM()
{
    VPLSamplePattern = 0;
    VPLSampleTest = 0;
}
//----------------------------------------------------------------------------
void VPLSampleRSM::OnPreDispatch(unsigned int pass)
{
    VPLSamplePattern->BindToImageUnit(0, GL_READ_ONLY);
    VPLSampleTest->BindToImageUnit(1, GL_WRITE_ONLY);
}
//----------------------------------------------------------------------------
void VPLSampleRSM::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    float* srcData = new float[16 * 4];
    float* dstData = new float[16 * 4];

    VPLSamplePattern->GetDataFromGPUMemory(srcData);
    VPLSampleTest->GetDataFromGPUMemory(dstData);

    int res = memcmp(srcData, dstData, 256);

    delete[] srcData;
    delete[] dstData;
}
//----------------------------------------------------------------------------