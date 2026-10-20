
#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Graphics/HUNTMesh/Mesh.h>
#include <HUNT/Graphics/GPU/MeshGPUBuffers.h>
#include <HUNT/Graphics/GPU/ShaderBuffers.h>

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
        //SDL manages these so raw is OK???
        SDL_GPURenderPass* renderPass = nullptr;
        SDL_GPUCommandBuffer* commandBuffer = nullptr;
        //  std::unique_ptr<SDL_GPUGraphicsPipeline, GPUPipelineDeleter> gpuPipeline;
        SDL_GPUGraphicsPipeline* gpuPipeline = nullptr;
        SDL_GPUTexture* swapchainTexture = nullptr;
        HUNT::MatrixBufferType cameraMatrix{
        .world = HUNT::mat4(1.f),
        .view = HUNT::mat4(1.f),
        .projection = HUNT::mat4(1.f),
        };
        std::unordered_map<uint32_t, MeshGPUBuffers> allStoredMeshBuffers;

        ~Impl() { Shutdown(); }

        void Shutdown() noexcept {
           
            //Cleanup for all the cached mesh buffers and their raw pointers (SDL managers so has to be raw)
            for (auto& [meshID, buffers] : allStoredMeshBuffers)
            {
                if (buffers.vertexBuffer)
                    SDL_ReleaseGPUBuffer(
                        gpuDevice.get(), buffers.vertexBuffer);

                if (buffers.indexBuffer)
                    SDL_ReleaseGPUBuffer(
                        gpuDevice.get(), buffers.indexBuffer);
            }

            allStoredMeshBuffers.clear();
            //Cleanup gpu pipeline stuff
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



    MeshGPUBuffers& Renderer::GetMeshBuffers(HUNT::Mesh& meshIn)
    {
        auto meshData = meshIn.getMeshData();

        //Attempt to get the mesh
        auto finalMesh = impl_->allStoredMeshBuffers.find(meshIn.getID());
        //If it exsits retun its buffers
        if (finalMesh != impl_->allStoredMeshBuffers.end()) {
            return finalMesh->second;
        }

        //if doesnt exsit make new buffers
        HUNT::MeshGPUBuffers newGPUBuffers;
        newGPUBuffers.vertexBuffer = CreateVertexBuffer(meshData.vertices);
        newGPUBuffers.indexBuffer = CreateIndexBuffer(meshData.indices);
        //and store them at the same ID returning the newly made buffers 
        auto [a, b] = impl_->allStoredMeshBuffers.emplace(meshIn.getID(), std::move(newGPUBuffers));
        //idk if its faster to return it FROM the structure or just what I made ill do this for now
        return a->second;

    }

    void Renderer::clearMeshBuffers()
    {
    }

    void Renderer::SetCameraMatrix(const MatrixBufferType& matrices)
    {
        impl_->cameraMatrix = matrices;
    }

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
   
    
    //Helper function to avoid being in header and repeating code for index and verrtex buffer
    SDL_GPUBuffer* CreateBuffer(SDL_GPUDevice* gpuDevice,const void* bufferDataIn, Uint32 size, SDL_GPUBufferUsageFlags usage) {

          Uint32 dataInSize = static_cast<Uint32>(size);
        //Make vertex buffer based on size
        SDL_GPUBufferCreateInfo bufferCreateInfo = SDL_GPUBufferCreateInfo{
            .usage = usage,
            .size = dataInSize,
        };

        SDL_GPUBuffer* gpuBuffer = SDL_CreateGPUBuffer(gpuDevice, &bufferCreateInfo);
        if (gpuBuffer == nullptr)
        {
            SDL_Log("Couldn't create vertex buffer: %s", SDL_GetError());
            return nullptr;
        }



        //TRANSFER BUFFER DIF CALL MAYBE?
        SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = SDL_GPUTransferBufferCreateInfo{
          .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
          .size = size,
        };
        SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(gpuDevice, &transferBufferCreateInfo);
        if (transferBuffer == nullptr)
        {
            SDL_Log("Couldn't create transfer buffer: %s", SDL_GetError());
            return nullptr; //temporary
            
        }

       // Vertex* transferData = static_cast<Vertex*>(SDL_MapGPUTransferBuffer(gpuDevice, transferBuffer, false));
        void* mappedTransferBuffer = SDL_MapGPUTransferBuffer(gpuDevice, transferBuffer, false);
        if (mappedTransferBuffer == nullptr)
        {
            SDL_Log("Couldn't map transfer buffer: %s", SDL_GetError());
            SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);
           
        }


        SDL_memcpy(mappedTransferBuffer, bufferDataIn, size); //Copy verticies into the memory the transfer buffer mapped for us

        SDL_UnmapGPUTransferBuffer(gpuDevice, transferBuffer); //unmap the transfer buffer pointer as not needed


        //Command buffer to upload to the vertex buffer

        SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(gpuDevice);
        if (uploadCmdBuf == nullptr)
        {
            SDL_Log("Couldn't acquire GPU command buffer: %s", SDL_GetError());
            
        }

        SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

        SDL_GPUTransferBufferLocation bufferLocation = SDL_GPUTransferBufferLocation{
            .transfer_buffer = transferBuffer,
            .offset = 0, };

        SDL_GPUBufferRegion bufferRegion = SDL_GPUBufferRegion{
            .buffer = gpuBuffer ,
            .offset = 0,
            .size = size, };


        SDL_UploadToGPUBuffer(copyPass, &bufferLocation, &bufferRegion, false);


        SDL_EndGPUCopyPass(copyPass);
        if (!SDL_SubmitGPUCommandBuffer(uploadCmdBuf))
        {
            SDL_Log("Couldn't submit GPU command buffer: %s", SDL_GetError());
            
        }

        SDL_ReleaseGPUTransferBuffer(gpuDevice, transferBuffer);

        return gpuBuffer;


    }

    SDL_GPUBuffer* Renderer::CreateVertexBuffer(std::span<Vertex>verticies) //ASSIGN VERTEX BUFFER
    {
        //TODO::ADD FALLBACKS FOR EMPTY MESH OR STRUCTURE EHRE

        return CreateBuffer(impl_->gpuDevice.get(),verticies.data(), verticies.size_bytes(), SDL_GPU_BUFFERUSAGE_VERTEX);
    }

    SDL_GPUBuffer* Renderer::CreateIndexBuffer(std::span<std::uint32_t> indices) //ASSIGN INDEX BUFFER
    {
        //TODO::ADD FALLBACKS FOR EMPTY MESH OR STRUCTURE EHRE
        return CreateBuffer(impl_->gpuDevice.get(), indices.data(), indices.size_bytes(), SDL_GPU_BUFFERUSAGE_INDEX);
    }
    void Renderer::BeginFrame()
    {
        //Make command buffer and swapchain texture
        impl_->commandBuffer = (SDL_AcquireGPUCommandBuffer(impl_->gpuDevice.get()));
        

        if (!impl_->commandBuffer || !SDL_WaitAndAcquireGPUSwapchainTexture(impl_->commandBuffer, impl_->window, &impl_->swapchainTexture, nullptr, nullptr))
        {
            // Explode
        }

        SDL_GPUColorTargetInfo colorTargetInfo = {};
        colorTargetInfo.texture = impl_->swapchainTexture;
        colorTargetInfo.clear_color = { impl_->defaultColour.x, impl_->defaultColour.y,impl_->defaultColour.z,impl_->defaultColour.w};
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;


        impl_->renderPass = (SDL_BeginGPURenderPass(impl_->commandBuffer, &colorTargetInfo, 1, nullptr)); //Begin Render Pass.
    
    }

    void Renderer::Render(HUNT::Mesh& meshIn)
    {
        //TODO::ADD FALLBACKS FOR EMPTY MESH OR STRUCTURE EHRE
        if (impl_->renderPass == nullptr ||
            impl_->commandBuffer == nullptr ||
            impl_->gpuPipeline == nullptr)
        {
            return;
        }


        HUNT::MeshGPUBuffers& buffer = GetMeshBuffers(meshIn);

        SDL_BindGPUGraphicsPipeline(impl_->renderPass,impl_->gpuPipeline);
        //Make vertex buffer using mesh and add it to the list to be culled later
        //SDL_GPUBuffer* vertexBuffer = CreateVertexBuffer(meshInVerticies);
        //impl_->frameVertexBuffers.push_back(vertexBuffer);


        std::array vertexBufferBound{
            SDL_GPUBufferBinding{
                .buffer = buffer.vertexBuffer,
                .offset = 0,
            },
        };

        SDL_BindGPUVertexBuffers(impl_->renderPass, 0, vertexBufferBound.data(), vertexBufferBound.size()); //plural but below isnt?
        
        
       /* SDL_GPUBuffer* indexBuffer = CreateIndexBuffer(meshInIndices);
        impl_->frameVertexBuffers.push_back(indexBuffer);*/
        std::array indexBufferBound{
           SDL_GPUBufferBinding{
               .buffer = buffer.indexBuffer,
               .offset = 0,
           },
        };
        SDL_BindGPUIndexBuffer(impl_->renderPass,indexBufferBound.data(), SDL_GPU_INDEXELEMENTSIZE_32BIT);

       // SDL_PushGPUVertexUniformData(impl_->commandBuffer, 0, &impl_->cameraMatrix, sizeof(HUNT::MatrixBufferType));

        SDL_DrawGPUIndexedPrimitives(impl_->renderPass, meshIn.getMeshData().GetIndexCount(), 1, 0, 0, 0);


       // SDL_Log("%zu", impl_->allStoredMeshBuffers.size());
    }






    void Renderer::EndFrame() {

        SDL_EndGPURenderPass(impl_->renderPass); //End Render Pass.
        impl_->renderPass = nullptr;

        SDL_SubmitGPUCommandBuffer(impl_->commandBuffer); //Submit to GPU.
        impl_->commandBuffer = nullptr;

        impl_->swapchainTexture = nullptr;
    }


}
