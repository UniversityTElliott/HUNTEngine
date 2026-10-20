#pragma once

#include <cstdint>
#include <SDL3/SDL_gpu.h>



namespace HUNT {

	struct MeshGPUBuffers {
		SDL_GPUBuffer* vertexBuffer = nullptr;
		SDL_GPUBuffer* indexBuffer = nullptr;

	};
}
