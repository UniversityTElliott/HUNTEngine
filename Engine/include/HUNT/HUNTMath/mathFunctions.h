#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <HUNT/HUNTMath/mat4.h>

namespace HUNT
{
    inline HUNT::mat4 lookAtLH(const glm::vec3& eye,const glm::vec3& centre,const glm::vec3& up) //literally just GLMs but for hunt
    {
        return glm::lookAtLH(eye, centre, up);
    }

    inline glm::mat4 perspectiveLH(float fovy,float aspect,float zNear,float zFar) //literally just GLMs but for hunt
    {
        return glm::perspectiveLH(fovy, aspect, zNear, zFar);
    }

    inline float toRadians(const float degreesIn) //literally just GLMs but for hunt
    {
        return glm::radians(degreesIn);
    }
    inline HUNT::float3 cross(const HUNT::float3 vecA, const HUNT::float3 vecB) //literally just GLMs but for hunt
    {
        return glm::cross(vecA,vecB);
    }
    inline HUNT::float3 normalise(const HUNT::float3 vecA) //literally just GLMs but for hunt FINALLY I CAN USE THE BRITISH SPELLING!
    {
        return glm::normalize(vecA);
    }
}