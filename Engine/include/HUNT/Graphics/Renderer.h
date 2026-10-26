#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include"HUNTMesh/MeshData.h"
#include"HUNTMesh/Mesh.h"
#include "HUNT/Graphics/HUNTMesh/Texture.h"
#include"GPU/MeshGPUBuffers.h"
#include"GPU/ShaderBuffers.h"
//https://gpuforbeginners.com/
//https://github.com/TheSpydog/SDL_gpu_examples/blob/main/Examples/TexturedQuad.c


struct SDL_Window;
struct SDL_GPUBuffer;

namespace HUNT {

    class Renderer {
    public:
         Renderer(SDL_Window* window); //SDL IN HEADER WE DONT WANT. UNSURE OF SOLUTION
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        MeshGPUBuffers& GetMeshBuffers(HUNT::Mesh& meshIn); //this will also make them if they dont exist
        void clearMeshBuffers();
        void SetCameraMatrix(const MatrixBufferType& matrices);


        void SetVSync(bool enabled);
        void InitGPU();
        void CreatePipeline();
        SDL_GPUBuffer* CreateVertexBuffer(std::span<Vertex>verticies);
        SDL_GPUBuffer* CreateIndexBuffer(std::span<std::uint32_t> indices);
        HUNT::Texture* LoadTexture(const char* filePath);

        void BeginFrame();
        void Render(HUNT::Mesh& meshIn);
        void EndFrame();


        //add set bg cvolour

         //CURRENT PLAN FOR TEXTURE
        //MAKE A LOADTEX FUNCTION IN THE RENDERER THAT TAKES IN A FILE PATH
        //IT USES IMG_LoadGPUTexture() AND ADDS THE TEXTURE TO A MAP? OF THEM
        //RETURNS A COPY OF THE POINTER TO THIS
        //MAYBE ADD the zip bullshit

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}