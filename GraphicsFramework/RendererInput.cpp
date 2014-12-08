//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "RendererInput.h"

using namespace RTGI;

GLenum gsTextureTargets[Texture::TT_Max] =
{
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_2D_ARRAY
};

GLenum gsFilterType[FT_Max] =
{
    GL_NEAREST,
    GL_LINEAR
};

GLenum gsWrapType[WT_Max] =
{
    GL_CLAMP,
    GL_REPEAT
};

//----------------------------------------------------------------------------
void EnableTexture(RendererInput* input)
{
    Texture* texture = (Texture*)(BufferBase*)input->InputBuffer;
    GLuint t = texture->GetTexture();
    Texture::TextureType type = texture->GetType();
    GLenum target = gsTextureTargets[(int)type];
    glActiveTexture(GL_TEXTURE0 + input->View.BindingSlot);
    glBindTexture(target, t);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, 
        gsFilterType[(int)input->View.Sampler.MinFilter]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER,
        gsFilterType[(int)input->View.Sampler.MagFilter]);
    glTexParameteri(target, GL_TEXTURE_WRAP_S,
        gsWrapType[(int)input->View.Sampler.WrapS]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T,
        gsWrapType[(int)input->View.Sampler.WrapT]);
}
//----------------------------------------------------------------------------
void EnableBuffer(RendererInput* input)
{
    Buffer* buffer = (Buffer*)(BufferBase*)input->InputBuffer;
    switch( input->View.BindingType )
    {
    case BF_Bind:
        buffer->Bind();
        break;

    case BF_BindIndex:
        buffer->Bind(input->View.BindingSlot);
        break;

    case BF_BindToIndirect:
        buffer->BindToIndirect();
        break;

    default:
        assert(false);
        break;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
RendererInput::RendererInput(const std::string& name,
    BufferBase* inputBuffer, RendererInputDataView* view)
{
    Name = name;
    InputBuffer = inputBuffer;
    View = *view;
}
//----------------------------------------------------------------------------
RendererInput::~RendererInput()
{
    InputBuffer = 0;
}
//----------------------------------------------------------------------------
void RendererInput::Enable()
{
    switch( View.Type )
    {
    case RDT_Texture:
        EnableTexture(this);
        break;

    case RDT_StructuredBuffer:
    case RDT_AtomicCounterBuffer:
    case RDT_UniformBuffer:
    case RDT_DispatchIndirectBuffer:
    case RDT_PixelBuffer:
    case RDT_TextureBuffer:
        EnableBuffer(this);
        break;

    default:
        assert(false);
        break;
    }
}
//----------------------------------------------------------------------------
void RendererInput::Disable()
{
    // Nothing to do.
}
//----------------------------------------------------------------------------