#pragma once

#include <SDL3_image/SDL_image.h>
namespace HUNT
{
    struct Texture
    {
        SDL_GPUTexture* gpuTexture = nullptr;
        int width = 0;
        int height = 0;


        SDL_GPUTexture* GetGPUTexture() const
        {
            return gpuTexture;
        }
        int GetWidth() const
        { 
            return width; 
        }
        int GetHeight() const
        {
            return height;
        }
    };
}