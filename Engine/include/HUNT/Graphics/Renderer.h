#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include"HUNTMesh/Mesh.h"



struct SDL_Window;
struct SDL_GPUBuffer;



namespace HUNT {

    class Renderer {
    public:
         Renderer(SDL_Window* window); //SDL IN HEADER WE DONT WANT. UNSURE OF SOLUTION
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void SetVSync(bool enabled);
        void InitGPU();
        void CreatePipeline();
        SDL_GPUBuffer* CreateVertexBuffer(std::span<Vertex>verticies);
        SDL_GPUBuffer* CreateIndexBuffer(std::span<std::uint32_t> indices);
        void BeginFrame();
        void Render(HUNT::Mesh meshIn);
        void EndFrame();


        //add set bg cvolour

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}