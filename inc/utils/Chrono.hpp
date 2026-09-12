#pragma once

#include <ctime>
#include <iostream>
#include <string>

namespace mbl { namespace utils {

/// Monotonic stopwatch, in seconds.
class	Chrono
{
	public:
		Chrono()
		{
			start();
		}
		~Chrono() {}

		/// Resets the stopwatch to zero.
		void	start()
		{
			_start = getTime();
		}
		/// Seconds elapsed since start().
		double	get()
		{
			return (getTime() - _start);
		}

		/// Current monotonic clock time, in seconds.
		static double getTime()
		{
			double	res;
			struct timespec	current;
			clock_gettime(CLOCK_MONOTONIC, &current);
			res = (current.tv_sec) + (current.tv_nsec) * 1e-9;
			return (res);
		}
	private:
		double		_start = 0;
};
}}
