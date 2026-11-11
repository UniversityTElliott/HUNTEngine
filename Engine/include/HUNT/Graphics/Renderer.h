#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include"HUNTMesh/MeshData.h"
#include"HUNTMesh/Mesh.h"
#include "HUNT/Graphics/HUNTMesh/Texture.h"
#include"GPU/MeshGPUBuffers.h"
#include"GPU/ShaderBuffers.h"
#include <HUNT/Math/mat4.h>
//https://gpuforbeginners.com/
//https://github.com/TheSpydog/SDL_gpu_examples/blob/main/Examples/TexturedQuad.c


struct SDL_Window;
struct SDL_GPUBuffer;
struct SDL_GPUTexture;
union SDL_Event;

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
        void CreateViewportTexture();
        //ImGUI stuff
        void InitImGui();
        void ProcessImGuiEvent(const SDL_Event& event);
        void BeginImGuiFrame();
        //EditorUI stuff its kinda imgui so below it
        void SetViewportSize(int x, int y);
        SDL_GPUTexture* GetViewportTexture() const;
        void RemakeViewport();
        //Camera Stuff
        void SetCameraMatrix(const MatrixBufferType& matrices);
        void SetVSync(bool enabled);
        //Mesh stuff
        MeshGPUBuffers& GetMeshBuffers(HUNT::Mesh& meshIn); //this will also make them if they dont exist
        SDL_GPUBuffer* CreateVertexBuffer(std::span<Vertex>verticies);
        SDL_GPUBuffer* CreateIndexBuffer(std::span<std::uint32_t> indices);
        void clearMeshBuffers();
        //Texture Stuff
        uint32_t LoadTexture(const char* filePath);
        HUNT::Texture* GetLoadedTexture(uint32_t textureID);

        //Rendering block
        void BeginFrame();
        void Render(HUNT::Mesh& meshIn, const HUNT::mat4& worldMatrix);
        void EndFrame();
        
    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}