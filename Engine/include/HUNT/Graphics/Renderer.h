#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include"HUNTMesh/MeshData.h"
#include"HUNTMesh/Mesh.h"
#include"GPU/MeshGPUBuffers.h"
#include"GPU/ShaderBuffers.h"



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
        void BeginFrame();
        void Render(HUNT::Mesh& meshIn);
        void EndFrame();


        //add set bg cvolour

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}