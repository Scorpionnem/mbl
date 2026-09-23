#pragma once

#include <iostream>
#include <cmath>

/// Fixed-size numeric vector (position, direction, color, size, ...). N = component count, T = component type.
template <std::size_t N, typename T = float>
struct vec
{
    T data[N] = {};

    std::size_t size() const {return (N);}

    vec() : vec(0) {}
    vec(T f) {for (std::size_t i = 0; i < N; i++) data[i] = f;}
    vec(T x, T y) requires (N >= 2) : vec() {data[0] = x; data[1] = y;}
    vec(T x, T y, T z) requires (N >= 3) : vec() {data[0] = x; data[1] = y; data[2] = z;}
    vec(T x, T y, T z, T w) requires (N >= 4) : vec() {data[0] = x; data[1] = y; data[2] = z; data[3] = w;}
    /// Converts to/from another size/type; extra components are dropped, missing ones stay 0.
    template <std::size_t NN, typename TT>
    vec(const vec<NN, TT>& cpy)
    {
    	for (std::size_t i = 0; i < N; i++) data[i] = cpy.data[i];
    }
    ~vec() = default;

    bool    operator==(const vec<N, T>& b) const;
    bool    operator!=(const vec<N, T>& b) const {return (!(*this == b));};

    vec<N, T>   operator-() const {return (*this * -1);}

    vec<N, T>   operator+(const vec<N, T>& b) const;
    vec<N, T>&  operator+=(const vec<N, T>& b);

    vec<N, T>   operator-(const vec<N, T>& b) const;
    vec<N, T>&  operator-=(const vec<N, T>& b);

    vec<N, T>   operator*(const vec<N, T>& b) const;
    vec<N, T>&  operator*=(const vec<N, T>& b);

    vec<N, T>   operator/(const vec<N, T>& b) const;
    vec<N, T>&  operator/=(const vec<N, T>& b);

	/// Euclidean (L2) length.
	static constexpr T	length(const vec<N, T>& v)
	{
		T	acc = 0;

		for (std::size_t i = 0; i < N; i++)
			acc += v.data[i] * v.data[i];

		return (std::sqrt(acc));
	}

	constexpr double	distance(const vec<N, T>& v) const
	{
		double d;
		double value = 0.0;

		for (std::size_t i = 0; i < N; i++)
			value += (data[i] - v.data[i]) * (data[i] - v.data[i]);

		d = std::sqrt(value);
		return d;
	}

	constexpr T	length() const {return (vec<N, T>::length(*this));}

	/// Scaled to unit length; returns a zero vector for a zero-length input.
	static constexpr vec<N, T>	normalize(const vec<N, T>& v)
	{
		T	len = v.length();

		return (len > 0 ? v / len : vec<N, T>(0));
	}
	constexpr vec<N, T>	normalize() const {return (vec<N, T>::normalize(*this));}

	/// Cross product, N == 3 only.
	static constexpr vec<N, T>	cross(const vec<N, T>& v1, const vec<N, T>& v2) requires (N == 3)
	{
		return (vec<N, T>(
			v1.y() * v2.z() - v1.z() * v2.y(),
			v1.z() * v2.x() - v1.x() * v2.z(),
			v1.x() * v2.y() - v1.y() * v2.x()
		));
	}
	constexpr vec<N, T>	cross(const vec<N, T>& v) const requires (N == 3) {return (vec<N, T>::cross(*this, v));}

	static constexpr T	dot(const vec<N, T>& v1, const vec<N, T>& v2)
	{
		T	acc = 0;

		for (std::size_t i = 0; i < N; i++)
			acc += v1.data[i] * v2.data[i];

		return (acc);
	}
	constexpr T	dot(const vec<N, T>& v) const {return (vec<N, T>::dot(*this, v));}

	/// Reflects v across the plane whose normal is n (n should be unit length).
	static constexpr vec<N, T>	reflect(const vec<N, T>& v, const vec<N, T>& n)
	{
		return (v - vec<N, T>(2.0) * vec<N, T>(dot(n, v)) * n);
	}
	constexpr vec<N, T>	reflect(const vec<N, T>& n) const {return (vec<N, T>::reflect(*this, n));}

    T&  x() requires (N >= 1) {return (data[0]);}
    T&  y() requires (N >= 2) {return (data[1]);}
    T&  z() requires (N >= 3) {return (data[2]);}
    T&  w() requires (N >= 4) {return (data[3]);}

    T&  r() requires (N >= 1) {return (data[0]);}
    T&  g() requires (N >= 2) {return (data[1]);}
    T&  b() requires (N >= 3) {return (data[2]);}
    T&  a() requires (N >= 4) {return (data[3]);}

    T   x() const requires (N >= 1) {return (data[0]);}
    T   y() const requires (N >= 2) {return (data[1]);}
    T   z() const requires (N >= 3) {return (data[2]);}
    T   w() const requires (N >= 4) {return (data[3]);}

    T   r() const requires (N >= 1) {return (data[0]);}
    T   g() const requires (N >= 2) {return (data[1]);}
    T   b() const requires (N >= 3) {return (data[2]);}
    T   a() const requires (N >= 4) {return (data[3]);}
};

template <std::size_t N, typename T>
std::ostream&   operator<<(std::ostream& s, const vec<N, T>& v)
{
    s << "[";
    for (std::size_t i = 0; i < N; i++)
    {
        s << v.data[i];
        if (i != N - 1)
            s << ", ";
    }
    s << "]";
    return (s);
}

using vec2i = vec<2, int>;
using vec3i = vec<3, int>;
using vec4i = vec<4, int>;

using vec2f = vec<2, float>;
using vec3f = vec<3, float>;
using vec4f = vec<4, float>;

using vec2d = vec<2, double>;
using vec3d = vec<3, double>;
using vec4d = vec<4, double>;

template <std::size_t N, typename T>
bool vec<N, T>::operator==(const vec<N, T>& b) const
{
    for (std::size_t i = 0; i < N; i++)
        if (data[i] != b.data[i])
            return false;
    return true;
}

template <std::size_t N, typename T>
vec<N, T>    vec<N, T>::operator+(const vec<N, T>& b) const
{
    vec<N, T>   res;

    for (std::size_t i = 0; i < N; i++)
        res.data[i] = data[i] + b.data[i];

    return (res);
}

template <std::size_t N, typename T>
vec<N, T>&  vec<N, T>::operator+=(const vec<N, T>& b)
{
    *this = *this + b;
    return (*this);
}

template <std::size_t N, typename T>
vec<N, T>    vec<N, T>::operator-(const vec<N, T>& b) const
{
    vec<N, T>   res;

    for (std::size_t i = 0; i < N; i++)
        res.data[i] = data[i] - b.data[i];

    return (res);
}

template <std::size_t N, typename T>
vec<N, T>&  vec<N, T>::operator-=(const vec<N, T>& b)
{
    *this = *this - b;
    return (*this);
}

template <std::size_t N, typename T>
vec<N, T>    vec<N, T>::operator*(const vec<N, T>& b) const
{
    vec<N, T>   res;

    for (std::size_t i = 0; i < N; i++)
        res.data[i] = data[i] * b.data[i];

    return (res);
}

template <std::size_t N, typename T>
vec<N, T>&  vec<N, T>::operator*=(const vec<N, T>& b)
{
    *this = *this * b;
    return (*this);
}

template <std::size_t N, typename T>
vec<N, T>    vec<N, T>::operator/(const vec<N, T>& b) const
{
    vec<N, T>   res;

    for (std::size_t i = 0; i < N; i++)
        res.data[i] = data[i] / b.data[i];

    return (res);
}

template <std::size_t N, typename T>
vec<N, T>&  vec<N, T>::operator/=(const vec<N, T>& b)
{
    *this = *this / b;
    return (*this);
}
