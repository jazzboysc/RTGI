#include "VPLGenerator.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLGenerator::VPLGenerator(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
VPLGenerator::~VPLGenerator()
{
}
//----------------------------------------------------------------------------
void VPLGenerator::SetRSMRenderer(RSMRenderer* rsm)
{
    mRSM = rsm;

    RendererInputDataView view;
    view.Type = RDT_Texture;
    view.Sampler.MinFilter = FT_Nearest;
    view.Sampler.MagFilter = FT_Nearest;
    view.Sampler.WrapS = WT_Clamp;
    view.Sampler.WrapT = WT_Clamp;

    ClearInputDependency();

    view.BindingSlot = 0;
    AddInputDependency(rsm, "RSMPosition", &view);

    view.BindingSlot = 1;
    AddInputDependency(rsm, "RSMNormal", &view);

    view.BindingSlot = 2;
    AddInputDependency(rsm, "RSMFlux", &view);
}
//----------------------------------------------------------------------------
RSMRenderer* VPLGenerator::GetRSMRenderer() const
{
    return mRSM;
}
//----------------------------------------------------------------------------