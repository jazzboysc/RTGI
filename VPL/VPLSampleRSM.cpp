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
}
//----------------------------------------------------------------------------