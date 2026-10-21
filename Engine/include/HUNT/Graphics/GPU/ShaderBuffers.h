#pragma once

#include <cstdint>

#include<HUNT/HUNTMath/mat4.h>


namespace HUNT {

	struct MatrixBufferType {
		HUNT::mat4 world;
		HUNT::mat4 view;
		HUNT::mat4 projection;
	};

	inline MatrixBufferType makeGPUMatrix(HUNT::mat4 worldIn, HUNT::mat4 viewIn, HUNT::mat4 projIn) //literally just GLMs but for hunt
	{
		return MatrixBufferType{
		  .world = worldIn,
		  .view = viewIn,
		  .projection = projIn
		};
	}
}
