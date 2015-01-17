//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture1D.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture1D::Texture1D()
{
    mType = TT_Texture1D;
	Width = 0;
}
//----------------------------------------------------------------------------
Texture1D::~Texture1D()
{
}
//----------------------------------------------------------------------------
bool Texture1D::LoadFromSystemMemory(GPUDevice* device, 
    TextureInternalFormat internalFormat, int width, TextureFormat format, 
    TextureComponentType type, void* pixels)
{
    if( mTextureHandle )
    {
        return false;
    }

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture1DLoadFromSystemMemory)(
        this, internalFormat, width, format, type, pixels);

    Width = width;
    mInternalFormat = internalFormat;
    mFormat = format;
    mComponentType = type;

    return true;
}
//----------------------------------------------------------------------------
void Texture1D::UpdateFromPixelBuffer(PixelBuffer* pixelBuffer)
{
    GPU_DEVICE_FUNC(mTextureHandle->Device, Texture1DUpdateFromPixelBuffer)(
        this, pixelBuffer);
}
//--------------------------------------------------------------------------
void Texture1D::BindToImageUnit(unsigned int unit, BufferAccess access)
{
    GPU_DEVICE_FUNC(mTextureHandle->Device, TextureBindToImageUnit)(this, 
        unit, access);
}
//--------------------------------------------------------------------------
void Texture1D::CreateUniformRandomTexture(int sampleCount, int channelCount)
{
//    Width = sampleCount;
//    mType = GL_FLOAT;
//
//    switch( channelCount )
//    {
//    case 1:
//        mInternalFormat = GL_R32F;
//        mFormat = GL_R;
//        break;
//
//    case 2:
//        mInternalFormat = GL_RG32F;
//        mFormat = GL_RG;
//        break;
//
//    case 3:
//        mInternalFormat = GL_RGB32F;
//        mFormat = GL_RGB;
//        break;
//
//    case 4:
//        mInternalFormat = GL_RGBA32F;
//        mFormat = GL_RGBA;
//        break;
//
//    default:
//        assert(false);
//        break;
//    }
//
//    float* pixels = new float[sampleCount * channelCount];
//    for( int i = 0; i < sampleCount; ++i )
//    {
//        for( int j = 0; j < channelCount; ++j )
//        {
//            pixels[channelCount * i + j] = (float)UniformRandom();
//        }
//    }
//
//    glGenTextures(1, &mTexture);
//    glBindTexture(GL_TEXTURE_1D, mTexture);
//    glTexImage1D(GL_TEXTURE_1D, 0, mInternalFormat, sampleCount, 0, mFormat, 
//        mType, pixels);
//
//    delete[] pixels;
//
//    glBindTexture(GL_TEXTURE_1D, 0);
//
//#ifdef _DEBUG
//    GLenum res = glGetError();
//    assert(res == GL_NO_ERROR);
//#endif
}
//--------------------------------------------------------------------------
void Texture1D::GetDataFromGPUMemory(void* dstData)
{
    GPU_DEVICE_FUNC(mTextureHandle->Device, Texture1DGetDataFromGPUMemory)(
        this, dstData);
}
//--------------------------------------------------------------------------