#pragma once

#include <ctime>
#include <iostream>
#include <string>
#include <sys/time.h>
#include "math/math.hpp"

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

		/// Current time in milliseconds since the epoch, for RTT measurement.
		static u64	getTimestampMS()
		{
			struct timeval tp;
			gettimeofday(&tp, NULL);
			return (tp.tv_sec * 1000 + tp.tv_usec / 1000);
		}
	private:
		double		_start = 0;
};
}}
