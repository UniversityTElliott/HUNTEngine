
#pragma once
#include<HUNT/Math/float3.h>
#include<HUNT/Math/quat.h>
#include <HUNT/Objects/Components/HUNTBaseComponent.h>

namespace HUNT {
	class HUNTTransformComponent : public HUNTBaseComponent
	{
	public:
		HUNTTransformComponent() = default;
		~HUNTTransformComponent() = default;

		HUNT::float3 position = HUNT::float3(0.0f);
		HUNT::quat rotation = HUNT::quat(1.0f, 0.0f, 0.0f, 0.0f);
		HUNT::float3 scale = HUNT::float3(1.0f);
	};
}
