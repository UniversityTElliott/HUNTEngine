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
        //
        void resetDepthBuffer();
        //Initialisers
        void InitGPU();
        void CreatePipeline();
        void CreateSampler();
        void CreateDepthSampler();
        
        //Camera Stuff
        void SetCameraMatrix(const MatrixBufferType& matrices);
        void SetVSync(bool enabled);
        //Mesh stuff
        MeshGPUBuffers& GetMeshBuffers(HUNT::Mesh& meshIn); //this will also make them if they dont exist
        SDL_GPUBuffer* CreateVertexBuffer(std::span<Vertex>verticies);
        SDL_GPUBuffer* CreateIndexBuffer(std::span<std::uint32_t> indices);
        void clearMeshBuffers();
        //Texture Stuff
        void LoadTexture(const char* filePath);
        HUNT::Texture* GetLoadedTexture(const char* filePath);

        //Rendering block
        void BeginFrame();
        void Render(HUNT::Mesh& meshIn);
        void EndFrame();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}