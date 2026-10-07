#pragma once

#include <cstdint>
#include <memory>
#include <span>
#include "MeshData.h"

struct SDL_Window;

namespace HUNT {

    class Renderer {
    public:
         Renderer(SDL_Window* window); //SDL IN HEADER WE DONT WANT. UNSURE OF SOLUTION
        ~Renderer();

        Renderer(const Renderer&) = delete;
        Renderer& operator=(const Renderer&) = delete;

        void SetVSync(bool enabled);
        void InitGPU();
        void UpdateGPU();
        void CreatePipeline();
        void CreateVertexBuffer(std::span<Vertex>verticies);
        void Render();
        void Present();
        void    Clear(std::uint8_t red, std::uint8_t green, std::uint8_t blue);
        //Rectangle maker
        void drawRectangle();

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

}