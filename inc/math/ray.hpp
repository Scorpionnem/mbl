#pragma once

#include "math/vec.hpp"

template <std::size_t N, typename T = float>
struct ray
{
	vec<N, T>	orig;
	vec<N, T>	dir;
};
