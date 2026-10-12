

#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Graphics/HUNTMesh/MeshData.h>
#include <SDL3/SDL.h>
#include <array>
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include<filesystem>
#include <span>

namespace HUNT {

    struct Renderer::Impl {

        SDL_Window* window;  // borrowed



        struct RendererDeleter {
            void operator()(SDL_Renderer* renderer) const noexcept { SDL_DestroyRenderer(renderer); }
        };
        struct GPUDeviceDeleter {
            void operator()(SDL_GPUDevice* GPUDevice) const noexcept { SDL_DestroyGPUDevice(GPUDevice); }
        };
        /*  struct GPUPipelineDeleter {
              void operator()(SDL_GPUDevice* GPUDevice, SDL_GPUGraphicsPipeline* gpuPipeline) const noexcept {SDL_ReleaseGPUGraphicsPipeline(GPUDevice,gpuPipeline); }
          };*/

       HUNT::float4 defaultColour{0.f,0.f,0.f,1.f};

        std::unique_ptr<SDL_Renderer, RendererDeleter> renderer3D;
        std::unique_ptr<SDL_GPUDevice, GPUDeviceDeleter> gpuDevice;
        //SDL manages these so raw is OK
        SDL_GPURenderPass* renderPass = nullptr;
        SDL_GPUCommandBuffer* commandBuffer = nullptr;
        //  std::unique_ptr<SDL_GPUGraphicsPipeline, GPUPipelineDeleter> gpuPipeline;
        SDL_GPUGraphicsPipeline* gpuPipeline = nullptr;

        std::vector<SDL_GPUBuffer*> frameVertexBuffers;


        ~Impl() { Shutdown(); }

        void Shutdown() noexcept {
            //cleanup
            //Cleanup raw pointer
            if (gpuPipeline) {
                SDL_ReleaseGPUGraphicsPipeline(gpuDevice.get(), gpuPipeline);
                gpuPipeline = nullptr;
            }
            if (gpuPipeline) {
                SDL_ReleaseGPUGraphicsPipeline(gpuDevice.get(), gpuPipeline);
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
        //CREATES GPU DEVICE
        SDL_GPUShaderFormat formatFlags = SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL;
        impl_->gpuDevice.reset(SDL_CreateGPUDevice(formatFlags, true, nullptr));
        if (impl_->gpuDevice == nullptr)
        {
            SDL_Log("1. Couldn't create GPU device: %s", SDL_GetError());
            impl_->gpuDevice.reset();

        }
        //ATTACHES GPU DEVICE TO WINDOW
        if (!SDL_ClaimWindowForGPUDevice(impl_->gpuDevice.get(), impl_->window))
        {
            SDL_Log("2. Couldn't claim window for GPU device: %s", SDL_GetError());
            SDL_Log("SDL version: %d", SDL_GetVersion());
            impl_->gpuDevice.reset();
        }

        CreatePipeline();
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

    void Renderer::CreatePipeline() {
        //LOAD VERTEX SHADER
        SDL_GPUShader* vertexShader = LoadShader(impl_->gpuDevice.get(), "OnlyPosition.vert");
        if (vertexShader == nullptr)
        {
            SDL_Log("Couldn't create vertex shader!");
            impl_->Shutdown();
        }
        //LOAD FRAGMENT SHADER
        SDL_GPUShader* fragmentShader = LoadShader(impl_->gpuDevice.get(), "SolidColor.frag");
        if (fragmentShader == nullptr)
        {
            SDL_Log("Couldn't create fragment shader!");
            impl_->Shutdown();
        }
        //lOAD ALL OTHER SHADERS BELOW
         
    



        //Blocks off chunks of memory of size vertex, basically says each vertex is that much data in memory
        std::array vertexBufferDescriptions{
    SDL_GPUVertexBufferDescription{
        .slot = 0,
        .pitch = sizeof(Vertex),
        .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
        .instance_step_rate = 0,
    },
        };

        //Inside the chunks says what data is what
        //First set is position and its a float3
        std::array vertexAttributes{
    SDL_GPUVertexAttribute{
        .location = 0,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        .offset = offsetof(Vertex, position),
    },
    //next set is colour and its a float4
    SDL_GPUVertexAttribute{
       .location = 1,
       .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
         .offset = offsetof(Vertex, colour),
    },
        };

        std::array colorTargetDescriptions{
    SDL_GPUColorTargetDescription{
        .format = SDL_GetGPUSwapchainTextureFormat(impl_->gpuDevice.get(),impl_->window)
    }
        };

        //Build the grapphics pipeline with the afformentioned set parts
        SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = SDL_GPUGraphicsPipelineCreateInfo{
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .vertex_input_state = SDL_GPUVertexInputState{
            .vertex_buffer_descriptions = vertexBufferDescriptions.data(),
            .num_vertex_buffers = vertexBufferDescriptions.size(),
            .vertex_attributes = vertexAttributes.data(),
            .num_vertex_attributes = vertexAttributes.size(),
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST, //Allow this to change
        .rasterizer_state = SDL_GPURasterizerState{
            .fill_mode = SDL_GPU_FILLMODE_FILL,
        },
        .target_info = SDL_GPUGraphicsPipelineTargetInfo{
            .color_target_descriptions = colorTargetDescriptions.data(),
            .num_color_targets = colorTargetDescriptions.size(),
        },
        };

        //Actually create the pipeline using the set information above
        impl_->gpuPipeline = SDL_CreateGPUGraphicsPipeline(impl_->gpuDevice.get(), &pipelineCreateInfo);
        if (impl_->gpuPipeline == nullptr)
        {
            SDL_Log("Couldn't create graphics pipeline! %s", SDL_GetError());
            impl_->Shutdown();
        }

        //Release the shaders as theyve been set and added to the pipeline
        SDL_ReleaseGPUShader(impl_->gpuDevice.get(), vertexShader);
        SDL_ReleaseGPUShader(impl_->gpuDevice.get(), fragmentShader);
    }
   


    SDL_GPUBuffer* Renderer::CreateVertexBuffer(std::span<Vertex>verticies) //ASSIGN VERTEX BUFFER
    {
        //VERTEX BUFFER

        
        Uint32 verticesSize = verticies.size() * static_cast<Uint32>(sizeof(Vertex));
        //Make vertex buffer based on size
        SDL_GPUBufferCreateInfo vertexBufferCreateInfo = SDL_GPUBufferCreateInfo{
            .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = verticesSize,
        };

        SDL_GPUBuffer* vertexBuffer= SDL_CreateGPUBuffer(impl_->gpuDevice.get(), &vertexBufferCreateInfo);
        if (vertexBuffer == nullptr)
        {
            SDL_Log("Couldn't create vertex buffer: %s", SDL_GetError());
            impl_->Shutdown();
        }



        //TRANSFER BUFFER DIF CALL MAYBE?
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = SDL_GPUTransferBufferCreateInfo{
          .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
          .size = verticesSize,
        };
        SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(impl_->gpuDevice.get(), &transferBufferCreateInfo);
        if (transferBuffer == nullptr)
        {
            SDL_Log("Couldn't create transfer buffer: %s", SDL_GetError());
            impl_->Shutdown();
        }

        Vertex* transferData = static_cast<Vertex*>(SDL_MapGPUTransferBuffer(impl_->gpuDevice.get(),transferBuffer, false));
        if (transferData == nullptr)
        {
            SDL_Log("Couldn't map transfer buffer: %s", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(impl_->gpuDevice.get(), transferBuffer);
            impl_->Shutdown();
        }


        SDL_memcpy(transferData, verticies.data(), verticesSize); //Copy verticies into the memory the transfer buffer mapped for us

        SDL_UnmapGPUTransferBuffer(impl_->gpuDevice.get(), transferBuffer); //unmap the transfer buffer pointer as not needed


        //Command buffer to upload to the vertex buffer

        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(impl_->gpuDevice.get());
        if (uploadCmdBuf == nullptr)
        {
            SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
            impl_->Shutdown();
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

        SDL_GPUTransferBufferLocation bufferLocation = SDL_GPUTransferBufferLocation{
            .transfer_buffer = transferBuffer,
            .offset = 0,};

        SDL_GPUBufferRegion bufferRegion = SDL_GPUBufferRegion{
            .buffer = vertexBuffer ,
            .offset = 0,
            .size = verticesSize,};


        SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);


        SDL_EndGPUCopyPass(copyPass);
        if (!SDL_SubmitGPUCommandBuffer(uploadCmdBuf))
        {
            SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
            impl_->Shutdown();
        }

        SDL_ReleaseGPUTransferBuffer(impl_->gpuDevice.get(), transferBuffer);

        return vertexBuffer;
    }


    void Renderer::BeginFrame()
    {
        //Make command buffer and swapchain texture
        impl_->commandBuffer = (SDL_AcquireGPUCommandBuffer(impl_->gpuDevice.get()));
        SDL_GPUTexture* swapchainTexture;

        if (!impl_->commandBuffer || !SDL_WaitAndAcquireGPUSwapchainTexture(impl_->commandBuffer, impl_->window, &swapchainTexture, nullptr, nullptr))
        {
            // Explode
        }

        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.texture = swapchainTexture;
        colorTargetInfo.clear_color = { impl_->defaultColour.x, impl_->defaultColour.y,impl_->defaultColour.z,impl_->defaultColour.w};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;


        impl_->renderPass = (SDL_BeginGPURenderPass(impl_->commandBuffer, &colorTargetInfo, 1, nullptr)); //Begin Render Pass.
        swapchainTexture = nullptr;
    }

    void Renderer::Render(HUNT::Mesh meshIn)
    {
        auto meshInVerticies = meshIn.getMeshData().vertices;

        SDL_BindGPUGraphicsPipeline(impl_->renderPass,impl_->gpuPipeline);
        //Make vertex buffer using mesh and add it to the list to be culled later
        SDL_GPUBuffer* vertexBuffer = CreateVertexBuffer(meshInVerticies);
        impl_->frameVertexBuffers.push_back(vertexBuffer);

        std::array vertexBuffers{
            SDL_GPUBufferBinding{
                .buffer = vertexBuffer,
                .offset = 0,
            },
        };
        SDL_BindGPUVertexBuffers(impl_->renderPass, 0, vertexBuffers.data(), vertexBuffers.size());

        SDL_DrawGPUPrimitives(impl_->renderPass, meshInVerticies.size(), 1, 0, 0);


    }






    void Renderer::EndFrame() {

        SDL_EndGPURenderPass(impl_->renderPass); //End Render Pass.
        impl_->renderPass = nullptr;

        SDL_SubmitGPUCommandBuffer(impl_->commandBuffer); //Submit to GPU.
        impl_->commandBuffer = nullptr;


        //For each buffer release it on the GPU  CHANGE THIS LATER MAYBE REUSE?!
        for (SDL_GPUBuffer* buffer : impl_->frameVertexBuffers)
        {
            if (buffer)
            {
                SDL_ReleaseGPUBuffer(impl_->gpuDevice.get(), buffer);
            }
        }

        impl_->frameVertexBuffers.clear();
    }


}
