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
void Texture1D::CreateUniformRandomTexture(GPUDevice* device, 
    int sampleCount, int channelCount)
{
    Width = sampleCount;
    mComponentType = TCT_Float;

    switch( channelCount )
    {
    case 1:
        mInternalFormat = TIF_R32F;
        mFormat = TF_R;
        break;

    case 2:
        mInternalFormat = TIF_RG32F;
        mFormat = TF_RG;
        break;

    case 3:
        mInternalFormat = TIF_RGB32F;
        mFormat = TF_RGB;
        break;

    case 4:
        mInternalFormat = TIF_RGBA32F;
        mFormat = TF_RGBA;
        break;

    default:
        assert(false);
        break;
    }

    float* pixels = new float[sampleCount * channelCount];
    for( int i = 0; i < sampleCount; ++i )
    {
        for( int j = 0; j < channelCount; ++j )
        {
            pixels[channelCount * i + j] = (float)UniformRandom();
        }
    }

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture1DLoadFromSystemMemory)(
        this, mInternalFormat, Width, mFormat, mComponentType, pixels);

    delete[] pixels;
}
//--------------------------------------------------------------------------
void Texture1D::GetDataFromGPUMemory(void* dstData)
{
    GPU_DEVICE_FUNC(mTextureHandle->Device, Texture1DGetDataFromGPUMemory)(
        this, dstData);
}
//--------------------------------------------------------------------------