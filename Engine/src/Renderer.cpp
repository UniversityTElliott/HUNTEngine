
#include <HUNT/Graphics/Renderer.h>
#include <HUNT/Graphics/HUNTMesh/Mesh.h>
#include <HUNT/Graphics/GPU/MeshGPUBuffers.h>
#include <HUNT/Graphics/GPU/ShaderBuffers.h>

#include <glm/gtc/matrix_transform.hpp>

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>
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
       //ALL OF THIS NEEDS CLEANING PLEASE DO THAT!!!!



       HUNT::float4 defaultColour{0.f,0.f,0.f,1.f};
       int width = 0;
       int height = 0;
  
        std::unique_ptr<SDL_Renderer, RendererDeleter> renderer3D;
        std::unique_ptr<SDL_GPUDevice, GPUDeviceDeleter> gpuDevice;
        SDL_GPUTexture* depthTexture = nullptr;
        //SDL manages these so raw is OK???
        SDL_GPURenderPass* renderPass = nullptr;
        SDL_GPUCommandBuffer* renderCommandBuffer = nullptr;
   
        //  std::unique_ptr<SDL_GPUGraphicsPipeline, GPUPipelineDeleter> gpuPipeline;
        SDL_GPUGraphicsPipeline* gpuPipeline = nullptr;
        SDL_GPUSampler* textureSampler = nullptr;
        SDL_GPUTexture* swapchainTexture = nullptr;

        HUNT::MatrixBufferType cameraMatrix{
        .world = HUNT::mat4(1.f),
        .view = HUNT::mat4(1.f),
        .projection = glm::perspective(90.f,1080.f/720.f,0.1f,200.f), //BUILD THIS FROM PASSED IN DATA
        };

        SDL_GPUTexture* viewportTexture = nullptr;
        int viewportWidth = 0;
        int viewportHeight = 0;
        //So we dont change stuff mid updatae
        int newViewportWidth = 0;
        int newViewportHeight = 0;
        bool viewportNeedsResize = false;



        std::unordered_map<uint32_t, MeshGPUBuffers> allStoredMeshBuffers;
        std::vector<std::unique_ptr<HUNT::Texture>> loadedTextureVector;
     


        ~Impl() { Shutdown(); }

        void Shutdown() noexcept {
           
            //Cleanup for all the cached mesh buffers and their raw pointers (SDL managers so has to be raw)
            for (auto& [meshID, buffers] : allStoredMeshBuffers)
            {
                if (buffers.vertexBuffer) {
                    SDL_ReleaseGPUBuffer(gpuDevice.get(), buffers.vertexBuffer);
                }

                if (buffers.indexBuffer)
                {
                    SDL_ReleaseGPUBuffer(gpuDevice.get(), buffers.indexBuffer);
                }
            }

            allStoredMeshBuffers.clear();

            //loaded Textures
            for (auto& texture : loadedTextureVector)
            {
                if (texture && texture->GetGPUTexture())
                {
                    SDL_ReleaseGPUTexture(gpuDevice.get(),texture->GetGPUTexture());
                    texture->gpuTexture = nullptr;
                }
            }
            loadedTextureVector.clear();

            allStoredMeshBuffers.clear();
            //Samplers
            if (textureSampler)
            {
                SDL_ReleaseGPUSampler(gpuDevice.get(), textureSampler);
                textureSampler = nullptr;
            }
            //VP
            if (viewportTexture)
            {
                SDL_ReleaseGPUTexture(gpuDevice.get(),viewportTexture);
                viewportTexture = nullptr;
            }
            //DT
            if (depthTexture)
            {
                SDL_ReleaseGPUTexture(gpuDevice.get(), depthTexture);
                depthTexture = nullptr;
            }




            //CleanupIMGUI
            ImGui_ImplSDLGPU3_Shutdown();
            ImGui_ImplSDL3_Shutdown();

            ImGui::DestroyContext();


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


    void Renderer::resetDepthBuffer() {

        SDL_GetWindowSize(impl_->window, &impl_->width, &impl_->height);

        CreateDepthSampler();
   }

    //---------------------INITIALISE------------------------------------------------
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

        SDL_GetWindowSize(impl_->window, &impl_->width, &impl_->height);

        // Initially use the window dimensions
        impl_->viewportWidth = impl_->width;
        impl_->viewportHeight = impl_->height;

        impl_->newViewportWidth = impl_->viewportWidth;
        impl_->newViewportHeight = impl_->viewportHeight;
        
        CreateViewportTexture();
        CreateDepthSampler();
        CreateSampler();
        CreatePipeline();
        InitImGui();
        
    }
    //Helper function for the pipeline creation, makes
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
             .num_samplers = (stage == SDL_GPU_SHADERSTAGE_FRAGMENT) ? 1u : 0u, //1  sampler
             .num_storage_textures = 0,
             .num_storage_buffers = 0,
             .num_uniform_buffers = (stage == SDL_GPU_SHADERSTAGE_VERTEX) ? 1u : 0u, //1 cBuffer
        };


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


        //REMEMBER::ALL INPUT TO SHADER INFO IS BELOW!


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
    //next is UV and its a float 2
        SDL_GPUVertexAttribute{
        .location = 2,
        .buffer_slot = 0,
        .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
        .offset = offsetof(Vertex, UV),
    },
    //next is nroaml and its a float 3
    //        SDL_GPUVertexAttribute{
    //    .location = 3,
    //    .buffer_slot = 0,
    //    .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
    //    .offset = offsetof(Vertex, normal),
    //},
        };

        std::array colorTargetDescriptions{
    SDL_GPUColorTargetDescription{
        .format = SDL_GetGPUSwapchainTextureFormat(impl_->gpuDevice.get(),impl_->window)
    }
        };

        //Depth buffer stuffs




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
            .cull_mode = SDL_GPU_CULLMODE_BACK,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE,
        },
        .depth_stencil_state = {
        .compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL,
        .enable_depth_test = true,
        .enable_depth_write = true,
        },
        .target_info = SDL_GPUGraphicsPipelineTargetInfo{
            .color_target_descriptions = colorTargetDescriptions.data(),
            .num_color_targets = colorTargetDescriptions.size(),
            .depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
            .has_depth_stencil_target = true
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

    void Renderer::CreateSampler()
    {
        // ALSO ADD FLAGS OR SUMMUIN
        SDL_GPUSamplerCreateInfo samplerInfo{};
        samplerInfo.min_filter = SDL_GPU_FILTER_LINEAR;
        samplerInfo.mag_filter = SDL_GPU_FILTER_LINEAR;
        samplerInfo.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
        samplerInfo.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
        samplerInfo.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;

        impl_->textureSampler = SDL_CreateGPUSampler(impl_->gpuDevice.get(), &samplerInfo);

        if (!impl_->textureSampler)
        {
            SDL_Log("Failed to create texture sampler: %s", SDL_GetError());
            return;
        }
    }
    void Renderer::CreateDepthSampler()
    {
        //Was getting an error in debug with D3D12 issue so this fixes it
        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetFloatProperty(props,SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_DEPTH_FLOAT,1.0f);



        SDL_GPUTextureCreateInfo depthInfo = {
    .type = SDL_GPU_TEXTURETYPE_2D,
    .format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT,
    .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
    .width = static_cast<Uint32>(impl_->width),
    .height = static_cast<Uint32>(impl_->height),
    .layer_count_or_depth = 1,
    .num_levels = 1,
    .sample_count = SDL_GPU_SAMPLECOUNT_1,
    .props = props
    
        };

        impl_->depthTexture = (SDL_CreateGPUTexture(impl_->gpuDevice.get(), &depthInfo));
    }
    void Renderer::CreateViewportTexture()
    {
        //Error check for size
        if (impl_->viewportWidth <= 0 ||
            impl_->viewportHeight <= 0)
        {
            return;
        }
        //Says mismatch between SDL backend and mine so give the backend mine
        SDL_PropertiesID props = SDL_CreateProperties();
        SDL_SetFloatProperty(props,SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_R_FLOAT,impl_->defaultColour.x);
        SDL_SetFloatProperty(props,SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_G_FLOAT,impl_->defaultColour.y);
        SDL_SetFloatProperty(props,SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_B_FLOAT,impl_->defaultColour.z);
        SDL_SetFloatProperty(props,SDL_PROP_GPU_TEXTURE_CREATE_D3D12_CLEAR_A_FLOAT,impl_->defaultColour.w);



        SDL_GPUTextureCreateInfo viewportInfo{
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GetGPUSwapchainTextureFormat(impl_->gpuDevice.get(), impl_->window),
        .usage = SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<Uint32>(impl_->viewportWidth), //narrowing conversion
        .height = static_cast<Uint32>(impl_->viewportHeight),
        .layer_count_or_depth = 1,
        .num_levels =1,
        .sample_count = SDL_GPU_SAMPLECOUNT_1, //SAME AS IMGUI CHANGE WITH MORE SAMPLERS //MAYBE MAKE THIS SOMETHING THATS STORED AND INCREASES
        .props = props
        };

        impl_->viewportTexture =SDL_CreateGPUTexture(impl_->gpuDevice.get(),&viewportInfo);

        if (impl_->viewportTexture == nullptr)
        {
            SDL_Log("Failed to create viewport texture: %s",SDL_GetError());
        }

    }
    //---------------------IMGUI-------------------------------
    void Renderer::InitImGui()
    {
        //BoilerplateImgui with SDL3
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
        // SDL3 platform backend
        if (!ImGui_ImplSDL3_InitForSDLGPU(impl_->window))
        {
            SDL_Log("Failed to initialise ImGui");
            return;
        }


        ImGui_ImplSDLGPU3_InitInfo initInfo{};

        initInfo.Device = impl_->gpuDevice.get();
        initInfo.ColorTargetFormat =SDL_GetGPUSwapchainTextureFormat(impl_->gpuDevice.get(),impl_->window);

        initInfo.MSAASamples = SDL_GPU_SAMPLECOUNT_1; //CHANGE AS SAMPLE COUNT INCREASES!

        if (!ImGui_ImplSDLGPU3_Init(&initInfo))
        {
            SDL_Log("Failed to initialise ImGui SDL_GPU backend");
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            return;
        }

    }
    void Renderer::ProcessImGuiEvent(const SDL_Event& event) //keeps this inside renderer so engine doesnt need IMGUI info
    {
        ImGui_ImplSDL3_ProcessEvent(&event); 
    }
    void Renderer::BeginImGuiFrame()
    {
        //idk if I need both but works?
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }
    SDL_GPUTexture* Renderer::GetViewportTexture() const
    {
        return impl_->viewportTexture;
    }
    void Renderer::RemakeViewport()
    {
        //Early exit
        if (!impl_->viewportNeedsResize)
        {
            return;
        }

        //Release old stuff
        if (impl_->viewportTexture)
        {
            SDL_ReleaseGPUTexture(impl_->gpuDevice.get(),impl_->viewportTexture);
            impl_->viewportTexture = nullptr;
        }
        if (impl_->depthTexture)
        {
            SDL_ReleaseGPUTexture(impl_->gpuDevice.get(),impl_->depthTexture);
            impl_->depthTexture = nullptr;
        }

        //Fix dimensions
        impl_->viewportWidth =impl_->newViewportWidth;
        impl_->viewportHeight =impl_->newViewportHeight;

        //Recreate for new VP
        CreateViewportTexture();
        CreateDepthSampler();

        impl_->viewportNeedsResize = false;
    }
    void Renderer::SetViewportSize(int x, int y)
    {
        //ignore if negative
        if (x <= 0 || y <= 0)
        {
            return;
        }
        //Ignore if same
        if (x == impl_->newViewportWidth && y == impl_->newViewportHeight)
        {
            return;
        }

        impl_->newViewportWidth = x;
        impl_->newViewportHeight = y;
        impl_->viewportNeedsResize = true;

    }
    //-----------------CAMERA--------------------------
    void Renderer::SetCameraMatrix(const MatrixBufferType& matrices)
    {
        impl_->cameraMatrix = matrices;
    }

    void Renderer::SetVSync(bool enabled)
    {
        SDL_SetRenderVSync(impl_->renderer3D.get(), enabled);
    }


    //--------------MESH---------------------


    MeshGPUBuffers& Renderer::GetMeshBuffers(HUNT::Mesh& meshIn)
    {
        auto meshData = meshIn.GetMeshData();

        //Attempt to get the mesh
        auto finalMesh = impl_->allStoredMeshBuffers.find(meshIn.GetID());
        //If it exsits retun its buffers
        if (finalMesh != impl_->allStoredMeshBuffers.end()) {
            return finalMesh->second;
        }

        //if doesnt exsit make new buffers
        HUNT::MeshGPUBuffers newGPUBuffers;
        newGPUBuffers.vertexBuffer = CreateVertexBuffer(meshData.vertices);
        newGPUBuffers.indexBuffer = CreateIndexBuffer(meshData.indices);
        //and store them at the same ID returning the newly made buffers 
        auto [a, b] = impl_->allStoredMeshBuffers.emplace(meshIn.GetID(), std::move(newGPUBuffers));
        //idk if its faster to return it FROM the structure or just what I made ill do this for now
        return a->second;

    }

    //Helper function to avoid being in header and repeating code for index and verrtex buffer
    SDL_GPUBuffer* CreateBuffer(SDL_GPUDevice* gpuDevice, const void* bufferDataIn, Uint32 size, SDL_GPUBufferUsageFlags usage) {

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

        return CreateBuffer(impl_->gpuDevice.get(), verticies.data(), verticies.size_bytes(), SDL_GPU_BUFFERUSAGE_VERTEX);
    }

    SDL_GPUBuffer* Renderer::CreateIndexBuffer(std::span<std::uint32_t> indices) //ASSIGN INDEX BUFFER
    {
        //TODO::ADD FALLBACKS FOR EMPTY MESH OR STRUCTURE EHRE
        return CreateBuffer(impl_->gpuDevice.get(), indices.data(), indices.size_bytes(), SDL_GPU_BUFFERUSAGE_INDEX);
    }

    void Renderer::clearMeshBuffers()
    {
    }

    //Helper function to load shaders
   

   //-------------------------TEXTURES---------------------------------

    uint32_t Renderer::LoadTexture(const char* filePath)
    {

        SDL_GPUCommandBuffer* textureCommandBuffer =SDL_AcquireGPUCommandBuffer(impl_->gpuDevice.get());
        //Make sure above was successful
        if (!textureCommandBuffer)
        {
            SDL_Log("Failed to find texture command buffer: %s",SDL_GetError());
            return -1;
        }
        SDL_GPUCopyPass* textureCopyPass = SDL_BeginGPUCopyPass(textureCommandBuffer);
        //Make sue above was successful
        if (!textureCopyPass)
        {
            SDL_CancelGPUCommandBuffer(textureCommandBuffer);
            return -1;;
        }
        //SDLGPU uses references to alter these as the size of the texutre? sure bro
        int width = 0;
        int height = 0;

        SDL_GPUTexture* texture = IMG_LoadGPUTexture(impl_->gpuDevice.get(), textureCopyPass,filePath,&width,&height);
        //Used Copy pass for all we need it for so kill it
        SDL_EndGPUCopyPass(textureCopyPass);

        SDL_SubmitGPUCommandBuffer(textureCommandBuffer); //add error checking to this?

        if (!texture)
        {
            SDL_CancelGPUCommandBuffer(textureCommandBuffer);

            SDL_Log("Failed to load '%s': %s",filePath, SDL_GetError());
            return -1;
        }
        auto newTexture = std::make_unique<HUNT::Texture>();
        newTexture->gpuTexture = texture;
        newTexture->width = width;
        newTexture->height = height;
        uint32_t textureID = impl_->loadedTextureVector.size();

        impl_->loadedTextureVector.push_back(std::move(newTexture));

        return textureID;
    }

    HUNT::Texture* Renderer::GetLoadedTexture(uint32_t textureID)
    {
        //IF its not in the vector it fails
        if (textureID >= impl_->loadedTextureVector.size())
        {
            return nullptr;
        }
        //send it
        return impl_->loadedTextureVector[textureID].get();
    }

    //----------------------BELOW HERE IS THE RENDERING CALLS---------------------------------------------------

    void Renderer::BeginFrame()
    {
        //This does any resize before we use the gpu IE safely
        RemakeViewport();

        //Make command buffer and swapchain texture
        impl_->renderCommandBuffer = (SDL_AcquireGPUCommandBuffer(impl_->gpuDevice.get()));
        
        if (!impl_->renderCommandBuffer || !SDL_WaitAndAcquireGPUSwapchainTexture(impl_->renderCommandBuffer, impl_->window, &impl_->swapchainTexture, nullptr, nullptr))
        {
            // Explode
        }
        //StartIMGUI
        BeginImGuiFrame();

        SDL_GPUColorTargetInfo colorTargetInfo = {
          .texture = impl_->viewportTexture,
          .clear_color = { impl_->defaultColour.x, impl_->defaultColour.y,impl_->defaultColour.z,impl_->defaultColour.w},
          .load_op = SDL_GPU_LOADOP_CLEAR,
          .store_op = SDL_GPU_STOREOP_STORE,

        };
        SDL_GPUDepthStencilTargetInfo depthTargetInfo = {
          .texture = impl_->depthTexture,
          .clear_depth = 1.0f,
         .load_op = SDL_GPU_LOADOP_CLEAR,
          .store_op = SDL_GPU_STOREOP_DONT_CARE
        };


        impl_->renderPass = (SDL_BeginGPURenderPass(impl_->renderCommandBuffer, &colorTargetInfo, 1, &depthTargetInfo)); //Begin Render Pass.
    
    }

    

    void Renderer::Render(HUNT::Mesh& meshIn, const HUNT::mat4& worldMatrix)
    {
        //TODO::ADD FALLBACKS FOR EMPTY MESH OR STRUCTURE EHRE
        if (impl_->renderPass == nullptr or impl_->renderCommandBuffer == nullptr or impl_->gpuPipeline == nullptr)
        {
            return;
        }


        HUNT::MeshGPUBuffers& buffer = GetMeshBuffers(meshIn);

        SDL_BindGPUGraphicsPipeline(impl_->renderPass, impl_->gpuPipeline);
        //Make vertex buffer using mesh and add it to the list to be culled later
        //SDL_GPUBuffer* vertexBuffer = CreateVertexBuffer(meshInVerticies);
        //impl_->frameVertexBuffers.push_back(vertexBuffer);


        HUNT::Texture* texture = meshIn.GetMaterial().GetTexture();
        
        if (texture!=nullptr && impl_->textureSampler)
        {
            SDL_GPUTextureSamplerBinding textureBinding{};

            textureBinding.texture = texture->GetGPUTexture();
            textureBinding.sampler = impl_->textureSampler;

            SDL_BindGPUFragmentSamplers(impl_->renderPass,0,&textureBinding,1); //0 = slot //1 = total
        }

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
        SDL_BindGPUIndexBuffer(impl_->renderPass, indexBufferBound.data(), SDL_GPU_INDEXELEMENTSIZE_32BIT);

   
        HUNT::MatrixBufferType matrices = impl_->cameraMatrix;
        matrices.world = worldMatrix;

        SDL_PushGPUVertexUniformData(impl_->renderCommandBuffer, 0, &matrices, sizeof(HUNT::MatrixBufferType));

        SDL_DrawGPUIndexedPrimitives(impl_->renderPass, meshIn.GetMeshData().GetIndexCount(), 1, 0, 0, 0);

    }
     






    void Renderer::EndFrame() {

        //OPTIMISE SO IT BATCHES STUFF AND ONLY SENDS IT ONCE TO GPU AT START OF HERE OR END OF ABOVE^^^

        SDL_EndGPURenderPass(impl_->renderPass); //End Render Pass.
        impl_->renderPass = nullptr;

        //ImGUI HERE

        ImGui::Render();
        ImDrawData* drawData = ImGui::GetDrawData();
        ImGui_ImplSDLGPU3_PrepareDrawData(drawData,impl_->renderCommandBuffer);
        //IMGUI needs to do another renderpass like week 6 graphics
        SDL_GPUColorTargetInfo imguiColorTarget{};
        imguiColorTarget.texture = impl_->swapchainTexture;
        imguiColorTarget.load_op = SDL_GPU_LOADOP_LOAD;
        imguiColorTarget.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPURenderPass* imguiRenderPass =SDL_BeginGPURenderPass(impl_->renderCommandBuffer,&imguiColorTarget,1,nullptr);
        if (imguiRenderPass)
        {
            ImGui_ImplSDLGPU3_RenderDrawData(drawData,impl_->renderCommandBuffer,imguiRenderPass);
            SDL_EndGPURenderPass(imguiRenderPass);
        }
         //Submit to GPU.
        SDL_SubmitGPUCommandBuffer(impl_->renderCommandBuffer);
        impl_->renderCommandBuffer = nullptr;
        impl_->swapchainTexture = nullptr;
    }


}
