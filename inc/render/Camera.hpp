#pragma once

#include "math/math.hpp"

namespace mbl { namespace render {

/// First-person style camera (position + yaw/pitch) with view/projection matrix helpers.
struct   Camera
{
    mat4f	getViewMatrix() const
    {
    	return (mat4f::rotateX(radians(-pitch)) * mat4f::rotateY(radians(yaw)) * mat4f::translate(-pos));
    }
    mat4f	getProjectionMatrix() const
    {
        return (mat4f::perspective(fov, aspect, near, far));
    }

    vec3f    front() const
    {
    	return (front(yaw, pitch));
    }
    static vec3f front(float yaw, float pitch)
    {
	    float   c = std::cos(radians(pitch));

	    return (vec3f(
	        std::sin(radians(yaw)) * c,
	        std::sin(radians(pitch)),
	        -std::cos(radians(yaw)) * c));
    }

    vec3f   pos;
    float  yaw = 0;
    float  pitch = 0;
    float  fov = 70;
    float  aspect = 1;
    float  near = 0.01;
    float  far = 1000;
};

}}
