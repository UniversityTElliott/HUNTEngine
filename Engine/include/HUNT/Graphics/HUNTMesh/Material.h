// Material.h
#pragma once

#include <SDL3_image/SDL_image.h>
#include <HUNT/HUNTMath/float4.h>

namespace HUNT
{
    using Texture = SDL_GPUTexture; //Not a good implement? idk probably fine

    class Material
    {
    public:
        Material() = default;

        void SetTexture(Texture* texture)
        {
            matTexture = texture;
        }

        Texture* GetTexture() const
        {
            return matTexture;
        }

        void SetColour(const glm::vec4& colour)
        {
            matColor = colour;
        }

        const glm::vec4& GetColour() const
        {
            return matColor;
        }

    private:

        //CURRENT PLAN FOR TEXTURE
        //MAKE A LOADTEX FUNCTION IN THE RENDERER THAT TAKES IN A FILE PATH
        //IT USES IMG_LoadGPUTexture() AND ADDS THE TEXTURE TO A MAP? OF THEM
        //RETURNS A COPY OF THE POINTER TO THIS
        //MAYBE ADD the zip bullshit

        Texture* matTexture = nullptr;
        HUNT::float4 matColor{ 1.0f, 1.0f, 1.0f, 1.0f };
    };
}