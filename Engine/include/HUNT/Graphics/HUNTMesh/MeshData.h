
#pragma once

#include <cstdint>
#include <HUNT/HUNTMath/float3.h>
#include <HUNT/HUNTMath/float4.h>

namespace HUNT {

	struct Vertex
	{
		HUNT::float3 position;
		HUNT::float4 colour;

	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

}

