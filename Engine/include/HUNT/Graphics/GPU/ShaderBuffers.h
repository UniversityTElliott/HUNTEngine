#pragma once

#include <cstdint>

#include<HUNT/HUNTMath/mat4.h>


namespace HUNT {

	struct MatrixBufferType {
		HUNT::mat4 world;
		HUNT::mat4 view;
		HUNT::mat4 projection;
		
		

	};
}
