#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <HUNT/Math/mat4.h>
#include <HUNT/Math/quat.h>

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
    inline HUNT::float3 vectorLerp(const HUNT::float3 vecA, const HUNT::float3 vecB, const float t) 
    {
        
        return HUNT::float3{ std::lerp(vecA.x, vecB.x, t), std::lerp(vecA.y, vecB.y, t), std::lerp(vecA.z, vecB.z, t) };
    }
    inline HUNT::quat quaternionSlerp(const HUNT::quat quatA, const HUNT::quat quatB, const float t)
    {
        return glm::slerp(quatA, quatB, t);
    }
    inline HUNT::mat4 calculateWorldMatrix(const HUNT::float3& position,const HUNT::quat& rotation,const HUNT::float3& scale)
    {
        HUNT::mat4 translationMatrix = glm::translate(glm::mat4(1.0f),position);

        HUNT::mat4 rotationMatrix = glm::mat4_cast(rotation);

        HUNT::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f),scale);

        return translationMatrix * rotationMatrix * scaleMatrix;
    }
}