// Material.h
#pragma once

#include <HUNT/Graphics/HUNTMesh/Texture.h>
#include <HUNT/HUNTMath/float4.h>

namespace HUNT
{
    class Material
    {
    public:
        Material() = default;

        void SetTexture(HUNT::Texture* texture)
        {
            matTexture = texture;
        }

        HUNT::Texture* GetTexture() const
        {
            return matTexture;
        }

        void SetColor(const  HUNT::float4& colour)
        {
            matColor = colour;
        }

        const  HUNT::float4& GetColor() const
        {
            return matColor;
        }

    private:

        //CURRENT PLAN FOR TEXTURE
        //MAKE A LOADTEX FUNCTION IN THE RENDERER THAT TAKES IN A FILE PATH
        //IT USES IMG_LoadGPUTexture() AND ADDS THE TEXTURE TO A MAP? OF THEM
        //RETURNS A COPY OF THE POINTER TO THIS
        //MAYBE ADD the zip bullshit

        HUNT::Texture* matTexture = nullptr;
        HUNT::float4 matColor{ 1.0f, 1.0f, 1.0f, 1.0f };
    };
}