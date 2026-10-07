

#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Graphics/MeshData.h>
#include <SDL3/SDL.h>
#include <array>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include<filesystem>

namespace HUNT {

    struct Renderer::Impl {
        
        SDL_Window* window;  // borrowed

        

        struct RendererDeleter {
            void operator()(SDL_Renderer* renderer) const noexcept { SDL_DestroyRenderer(renderer); }
        };
        struct GPUDeviceDeleter {
            void operator()(SDL_GPUDevice* GPUDevice) const noexcept { SDL_DestroyGPUDevice(GPUDevice); }
        };
     
       
        std::unique_ptr<SDL_Renderer, RendererDeleter> renderer3D;
        std::unique_ptr<SDL_GPUDevice, GPUDeviceDeleter> gpuDevice;
        SDL_GPUGraphicsPipeline* gpuPipeline = nullptr;
        
        
       
        ~Impl() { Shutdown(); }

        void Shutdown() noexcept {
            //cleanup
            //Cleanup raw pointer
            if (gpuPipeline) {
                SDL_ReleaseGPUGraphicsPipeline(gpuDevice.get(),gpuPipeline);
                gpuPipeline = nullptr;
            }

           SDL_ReleaseWindowFromGPUDevice(gpuDevice.get(), window);
           
            gpuDevice.reset();
            
        }

       
        explicit Impl(SDL_Window* window)
            : window(window)
        {
        }
    };
    Renderer::Renderer(SDL_Window* window)
        : impl_(std::make_unique<Impl>(window))
    {
    }
    Renderer::~Renderer() = default;


    void Renderer::SetVSync(bool enabled)
    {
       SDL_SetRenderVSync(impl_->renderer3D.get(), enabled);
    }
    void Renderer::InitGPU()
    {
        //Init GPU and window
        SDL_GPUShaderFormat formatFlags = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;
        impl_->gpuDevice.reset(SDL_CreateGPUDevice(formatFlags, true, nullptr));
        if (impl_->gpuDevice == nullptr)
        {
            SDL_Log("1. Couldn't create GPU device: %s", SDL_GetError());
            impl_->gpuDevice.reset();

        }

        if (!SDL_ClaimWindowForGPUDevice(impl_->gpuDevice.get(), impl_->window))
        {
            SDL_Log("2. Couldn't claim window for GPU device: %s", SDL_GetError());
            SDL_Log("SDL version: %d", SDL_GetVersion());
            impl_->gpuDevice.reset();
        }

    }


    //Helper function to load shaders
    SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const std::string& shaderFilename) {

        
        //Find out which stage it is
        SDL_GPUShaderStage stage;
        if (shaderFilename.find(".vert") != std::string::npos)
        {
            stage = SDL_GPU_SHADERSTAGE_VERTEX;
        }
        else if (shaderFilename.find(".frag") != std::string::npos)
        {
            stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
        }
        else
        {
            SDL_Log("Couldn't deduce shader stage from file name: %s", shaderFilename.c_str());
            return nullptr;
        }
        

        //Find which shader to load
        std::filesystem::path fullPath = std::filesystem::path(SDL_GetBasePath()) / "shaders";
        SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
        const char* entrypoint;


        SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
        if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV)
        {
            fullPath /= shaderFilename + ".spv";
            format = SDL_GPU_SHADERFORMAT_SPIRV;
            entrypoint = "main";
        }
        else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL)
        {
            fullPath /= shaderFilename + ".msl";
            format = SDL_GPU_SHADERFORMAT_MSL;
            entrypoint = "main0";
        }
        else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL)
        {
            fullPath /= shaderFilename + ".dxil";
            format = SDL_GPU_SHADERFORMAT_DXIL;
            entrypoint = "main";
        }
        else
        {
            SDL_Log("Couldn't find a supported shader format for backend %s!", SDL_GetGPUDeviceDriver(device));
            return nullptr;
        }

        //load shader from disk
        size_t fileSize;
        void* code = SDL_LoadFile(fullPath.string().c_str(), &fileSize);
        if (code == nullptr)
        {
            SDL_Log("Couldn't load shader file from disk!\n\t%s", SDL_GetError());
            return nullptr;
        }

        //ACTUALLY CREATE THE SHADER
     SDL_GPUShaderCreateInfo shaderInfo = SDL_GPUShaderCreateInfo{
    .code_size = fileSize,
    .code = static_cast<Uint8*>(code),
    .entrypoint = entrypoint,
    .format = format,
    .stage = stage,
        };
     SDL_Log("Loading shader: %s", shaderFilename.c_str());
     SDL_Log("  stage = %s",
         stage == SDL_GPU_SHADERSTAGE_VERTEX ? "VERTEX" : "FRAGMENT");
     SDL_Log("  format = %d", format);
     SDL_Log("  entrypoint = %s", entrypoint);
     SDL_Log("  path = %s", fullPath.string().c_str());
        SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
        if (shader == nullptr)
        {
            SDL_Log("Couldn't create shader from file %s: %s", fullPath.c_str(), SDL_GetError());
            SDL_free(code);
            return nullptr;
        }
        SDL_free(code);
        return shader;
    }

    bool Renderer::CreatePipeline() {
        //'Use helped to load the vertex shader
        SDL_GPUShader* vertexShader = LoadShader(impl_->gpuDevice.get(), "OnlyPosition.vert");
        if (vertexShader == nullptr)
        {
            SDL_Log("Couldn't create vertex shader!");
            return false;
        }
        //Then the fragment shader
        SDL_GPUShader* fragmentShader = LoadShader(impl_->gpuDevice.get(), "SolidColor.frag");
        if (fragmentShader == nullptr)
        {
            SDL_Log("Couldn't create fragment shader!");
            return false;
        }

        //Make vertex buffer
        std::array vertexBufferDescriptions{
    SDL_GPUVertexBufferDescription{
        .slot = 0,
        .pitch = sizeof(Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    },
        };


        std::array vertexAttributes{
    SDL_GPUVertexAttribute{
        .location = 0,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
        .offset = 0 * sizeof(float),
    },
    SDL_GPUVertexAttribute{
        .location = 1,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
        .offset = 1 * sizeof(float),
    },
    SDL_GPUVertexAttribute{
        .location = 2,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT,
        .offset = 2 * sizeof(float),
    },
        };

        std::array colorTargetDescriptions{
    SDL_GPUColorTargetDescription{
        .format = SDL_GetGPUSwapchainTextureFormat(impl_->gpuDevice.get(),impl_->window)
    }
        };


        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = SDL_GPUGraphicsPipelineCreateInfo{
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .vertex_input_state = SDL_GPUVertexInputState{
            .vertex_buffer_descriptions = vertexBufferDescriptions.data(),
            .num_vertex_buffers = vertexBufferDescriptions.size(),
            .vertex_attributes = vertexAttributes.data(),
            .num_vertex_attributes = vertexAttributes.size(),
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = SDL_GPURasterizerState{
            .fill_mode = SDL_GPU_FILLMODE_FILL,
        },
        .target_info = SDL_GPUGraphicsPipelineTargetInfo{
            .color_target_descriptions = colorTargetDescriptions.data(),
            .num_color_targets = colorTargetDescriptions.size(),
        },
        };
        impl_->gpuPipeline = SDL_CreateGPUGraphicsPipeline(impl_->gpuDevice.get(), &pipelineCreateInfo);
        if (impl_->gpuPipeline == nullptr)
        {
            SDL_Log("Couldn't create graphics pipeline! %s", SDL_GetError());
            return false;
        }


        SDL_ReleaseGPUShader(impl_->gpuDevice.get(), vertexShader);
        SDL_ReleaseGPUShader(impl_->gpuDevice.get(), fragmentShader);
    }


    void Renderer::UpdateGPU()
    {
        SDL_GPUCommandBuffer* commandBuffer = SDL_AcquireGPUCommandBuffer(impl_->gpuDevice.get());
        if (commandBuffer == nullptr)
        {
            SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
            //Explode
        }

        SDL_GPUTexture* swapchainTexture;
        if (!SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, impl_->window, &swapchainTexture, nullptr, nullptr))
        {
            SDL_Log("Couldn't acquire swapchain texture: %s", SDL_GetError());
            //Explode
        }

        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.texture = swapchainTexture;
        colorTargetInfo.clear_color = { 0.4f, 0.6f, 0.9f, 1.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;


        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, nullptr);
        SDL_EndGPURenderPass(renderPass);



        SDL_SubmitGPUCommandBuffer(commandBuffer);
    }




    void Renderer::Present() {

        SDL_RenderPresent(impl_->renderer3D.get());
    }

    void Renderer::Clear(std::uint8_t red, std::uint8_t green, std::uint8_t blue) {
        if (!impl_->renderer3D)
            throw std::logic_error("Engine is not initialised");
        bool result = SDL_SetRenderDrawColor(impl_->renderer3D.get(), red, green, blue, 255);
        result = SDL_RenderClear(impl_->renderer3D.get());
    }

    void Renderer::drawRectangle()
    {

        SDL_FRect rect;
        rect.x = 0.f;
        rect.y = 0.f;
        rect.w = 500.f;
        rect.h = 500.f;

        SDL_SetRenderDrawColor(impl_->renderer3D.get(), 255, 0, 0, 255);
        SDL_RenderFillRect(impl_->renderer3D.get(), &rect);
    }

}
