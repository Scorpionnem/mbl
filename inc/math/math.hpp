#pragma once

#include <cstdint>
#include <math/vec.hpp>

/// -1, 0 or 1 depending on the sign of n.
template <typename T>
int	sign(T n)
{
	return (n == 0 ? 0 : n > 0 ? 1 : -1);
}

/// Fractional part of v.
template <typename T>
T	frac(T v)
{
	return (v - floor(v));
}

/// Degrees to radians.
template <typename T>
inline T radians(T deg)
{
	return (deg * M_PI / 180.0);
}

/// Linear interpolation between a and b by t.
template <typename T, typename I>
T	lerp(T a, T b, I t)
{
	return (a + (b - a) * t);
}

#include <math/mat.hpp>

template <std::size_t N, typename T>
vec<N, T>	floor(const vec<N, T>& v)
{
	vec<N, T>	res;

	for (std::size_t i = 0; i < N; i++)
		res.data[i] = floor(v.data[i]);
	return (res);
}

template <std::size_t N, typename T>
vec<N, T>	abs(const vec<N, T>& v)
{
	vec<N, T>	res;

	for (std::size_t i = 0; i < N; i++)
		res.data[i] = std::abs(v.data[i]);
	return (res);
}

template <std::size_t N, typename T>
vec<N, T>	sign(const vec<N, T>& v)
{
	vec<N, T>	res;

	for (std::size_t i = 0; i < N; i++)
		res.data[i] = sign(v.data[i]);
	return (res);
}

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using vec2i8 = vec<2, i8>;
using vec3i8 = vec<3, i8>;
using vec4i8 = vec<4, i8>;
using vec2u8 = vec<2, u8>;
using vec3u8 = vec<3, u8>;
using vec4u8 = vec<4, u8>;

using vec2i16 = vec<2, i16>;
using vec3i16 = vec<3, i16>;
using vec4i16 = vec<4, i16>;
using vec2u16 = vec<2, u16>;
using vec3u16 = vec<3, u16>;
using vec4u16 = vec<4, u16>;

using vec2i32 = vec<2, i32>;
using vec3i32 = vec<3, i32>;
using vec4i32 = vec<4, i32>;
using vec2u32 = vec<2, u32>;
using vec3u32 = vec<3, u32>;
using vec4u32 = vec<4, u32>;

using vec2i64 = vec<2, i64>;
using vec3i64 = vec<3, i64>;
using vec4i64 = vec<4, i64>;
using vec2u64 = vec<2, u64>;
using vec3u64 = vec<3, u64>;
using vec4u64 = vec<4, u64>;
