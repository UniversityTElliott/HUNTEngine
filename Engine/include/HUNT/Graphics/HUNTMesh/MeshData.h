
#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include <HUNT/Math/float2.h>
#include <HUNT/Math/float3.h>
#include <HUNT/Math/float4.h>

namespace HUNT {



	struct Vertex
	{
		HUNT::float3 position;
		HUNT::float4 colour = HUNT::float4{ 1.0f, 1.0f, 1.0f, 1.0f };
		HUNT::float2 UV;
		//HUNT::float3 normal;

	};

	struct MeshData {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		size_t GetIndexCount() const //used in buffers//MAYBE PUT INTO MESH IDK TBH??!!?!
		{
			return indices.size();
		}

		
	};

}

