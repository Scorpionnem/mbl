#pragma once

#include <iostream>
#include "math/math.hpp"

/*
	column major

	3*3 matrix

	0 1 2
	3 4 5 -> 0 3 6  1 4 7  2 5 8
	6 7 8

	4*4 matirx
	0  1  2  3
	4  5  6  7  -> 0 4 8 12  1 5 9 13  2 6 10 14  3 7 11 15
	8  9  10 11
	12 13 14 15

	horizontal = row
	vertical = column
*/

/// Column-major N x N matrix (see mat3f/mat4f). translate/scale/rotate*/perspective/ortho/lookAt
/// are each only available for the N they make sense for, via `requires` clauses.
template <std::size_t N, typename T>
struct	mat
{
	T	data[N * N] = {};

	mat() {}

	static mat<N, T>	identity()
	{
		mat<N, T>	res;

		for (std::size_t i = 0; i < N; i++)
			res(i, i) = 1;

		return (res);
	}

	mat<N, T>	operator*(const mat<N, T> &m) const
	{
		mat<N, T>	res;

		for (std::size_t col = 0; col < N; col++)
		{
			for (std::size_t row = 0; row < N; row++)
			{
				for (std::size_t k = 0; k < N; k++)
					res(col, row) += (*this)(k, row) * m(col, k);
			}
		}

		return (res);
	}

	static mat<N, T>	translate(const vec<3, T> &v) requires (N == 4)
	{
		mat<N, T>	res = mat<N, T>::identity();

		res(3, 0) = v.x();
		res(3, 1) = v.y();
		res(3, 2) = v.z();
		return (res);
	}
	static mat<N, T>	translate(const vec<2, T> &v) requires (N == 3)
	{
		mat<N, T>	res = mat<N, T>::identity();

		res(2, 0) = v.x();
		res(2, 1) = v.y();
		return (res);
	}

	static mat<N, T>	scale(const vec<3, T> &v) requires (N == 4)
	{
		mat<N, T>	res = mat<N, T>::identity();

		res(0, 0) = v.x();
		res(1, 1) = v.y();
		res(2, 2) = v.z();
		return (res);
	}
	static mat<N, T>	scale(const vec<2, T> &v) requires (N == 3)
	{
		mat<N, T>	res = mat<N, T>::identity();

		res(0, 0) = v.x();
		res(1, 1) = v.y();
		return (res);
	}

	/// Rotation around the X axis, angle in radians.
	static mat<N, T>	rotateX(T angle) requires (N == 4)
	{
		mat<N, T>	res = mat<N, T>::identity();
		T c = std::cos(angle);
		T s = std::sin(angle);

		res(1, 1) =  c;
		res(1, 2) =  s;
		res(2, 1) = -s;
		res(2, 2) =  c;
		return (res);
	}

	/// Rotation around the Y axis, angle in radians.
	static mat<N, T>    rotateY(T angle) requires (N == 4)
	{
		mat<N, T>   res = mat<N, T>::identity();
		T c = std::cos(angle);
		T s = std::sin(angle);

		res(0, 0) =  c;
		res(0, 2) = -s;
		res(2, 0) =  s;
		res(2, 2) =  c;
		return (res);
	}

	/// Rotation around the Z axis, angle in radians.
	static mat<N, T>    rotateZ(T angle) requires (N == 4)
	{
		mat<N, T>   res = mat<N, T>::identity();
		T c = std::cos(angle);
		T s = std::sin(angle);

		res(0, 0) =  c;
		res(0, 1) =  s;
		res(1, 0) = -s;
		res(1, 1) =  c;
		return (res);
	}

	/// 2D rotation, angle in radians.
	static mat<N, T>    rotate(T angle) requires (N == 3)
	{
		mat<N, T>   res = mat<N, T>::identity();
		T c = std::cos(angle);
		T s = std::sin(angle);

		res(0, 0) =  c;
		res(0, 1) =  s;
		res(1, 0) = -s;
		res(1, 1) =  c;
		return (res);
	}

	/// OpenGL-style perspective projection (fov in degrees).
	static mat<N, T>    perspective(T fov_deg, T aspect, T near, T far) requires (N == 4)
	{
		T	tf2 = std::tan(radians(fov_deg) / 2);

		mat<N, T>	res;

		res(0, 0) = 1 / (aspect * tf2);
		res(1, 1) = 1 / tf2;
		res(2, 2) = -((far + near) / (far - near));
		res(3, 2) = -((2 * far * near) / (far - near));
		res(2, 3) = -1;
		return (res);
	}
	/// OpenGL-style orthographic projection.
	static mat<N, T>    ortho(T left, T right, T bottom, T top, T near, T far) requires (N == 4)
	{
	    mat<N, T>   res = mat<N, T>::identity();

		res(0, 0) = 2 / (right - left);
		res(1, 1) = 2 / (top - bottom);
		res(2, 2) = -2 / (far - near);

		res(3, 0) = -((right + left) / (right - left));
		res(3, 1) = -((top + bottom) / (top - bottom));
		res(3, 2) = -((far + near) / (far - near));

		return (res);
	}


	/// View matrix looking from eye towards center, with the given up vector.
	static mat<N, T>    lookAt(const vec<3, T> &eye, const vec<3, T> &center, const vec<3, T> &up) requires (N == 4)
	{
		vec<3, T>   f = (center - eye).normalize();
		vec<3, T>   s = f.cross(up).normalize();
		vec<3, T>   u = s.cross(f);

		mat<N, T>   res = mat<N, T>::identity();

		res(0, 0) =  s.x();
		res(1, 0) =  s.y();
		res(2, 0) =  s.z();

		res(0, 1) =  u.x();
		res(1, 1) =  u.y();
		res(2, 1) =  u.z();

		res(0, 2) = -f.x();
		res(1, 2) = -f.y();
		res(2, 2) = -f.z();

		res(3, 0) = -s.dot(eye);
		res(3, 1) = -u.dot(eye);
		res(3, 2) =  f.dot(eye);
		return (res);
	}

	T	&operator()(int col, int row)
	{
		return (data[row + col * N]);
	}
	T	operator()(int col, int row) const
	{
		return (data[row + col * N]);
	}
};

template <std::size_t N, typename T>
std::ostream&   operator<<(std::ostream& s, const mat<N, T>& v)
{
	for (std::size_t row = 0; row < N; row++)
	{
		s << "[";
		for (std::size_t col = 0; col < N; col++)
		{
			s << v(col, row);
			if (col != N - 1)
				s << ", ";
		}
		s << "]";
		if (row != N - 1)
			std::cout << '\n';
	}
    return (s);
}

using mat3f = mat<3, float>;
using mat4f = mat<4, float>;
