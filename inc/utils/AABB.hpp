#pragma once

#include "math/math.hpp"
#include "render/Mesh.hpp"
#include "render/Camera.hpp"

namespace mbl { namespace utils {

/// N-dimensional axis-aligned bounding box.
template <std::size_t N, typename T>
struct  AABB
{
    vec<N, T>   pos;
    vec<N, T>   size;

    static bool    intersects(const AABB<N, T>& a, const AABB<N, T>& b)
    {
        for (std::size_t i = 0; i < N; i++)
            if (a.pos.data[i] + a.size.data[i] < b.pos.data[i] || a.pos.data[i] > b.pos.data[i] + b.size.data[i])
                return (false);
        return (true);
    }

    static bool    contains(const AABB<N, T>& box, const vec<N, T>& point)
    {
        for (std::size_t i = 0; i < N; i++)
            if (point.data[i] < box.pos.data[i] || point.data[i] > box.pos.data[i] + box.size.data[i])
                return (false);
        return (true);
    }
};

using aabb2i = AABB<2, int>;
using aabb3i = AABB<3, int>;

using aabb2f = AABB<2, float>;
using aabb3f = AABB<3, float>;
}}
