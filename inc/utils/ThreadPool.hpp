#pragma once

#include <cstdint>
#include <deque>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <atomic>
#include <functional>
#include <unistd.h>

namespace mbl { namespace utils {

/// Fixed pool of worker threads pulling tasks off a shared queue.
class	ThreadPool
{
	public:
		ThreadPool() : _active_tasks(0), _stop(false) {}
		/// Constructs the pool and starts n worker threads.
		ThreadPool(uint64_t n) {add(n);}
		~ThreadPool() {if (!_stop) stop();}

		uint64_t	threads() {return (_threads_count);}

		/// Starts n more worker threads.
		void	add(uint64_t n)
		{
			_threads_count += n;
			for (uint32_t i = 0; i < n; ++i)
				_worker_threads.emplace_back(std::bind(&ThreadPool::_worker, this));
		}

		/// Signals all worker threads to stop and joins them.
		void	stop()
		{
			std::unique_lock<std::mutex> latch(_queue_mutex);

			_stop = true;
			_cv_task.notify_all();

			latch.unlock();

			for (std::thread &thread : _worker_threads)
				thread.join();
		}

		/// Blocks until the queue is empty and no task is currently running.
		void	wait_finish()
		{
			while (1)
			{
				_queue_mutex.lock();
				if (_tasks.empty() && _active_tasks == 0)
					break ;
				_queue_mutex.unlock();
				usleep(50);
			}
			_queue_mutex.unlock();
		}
		uint64_t	active_tasks() {return (_active_tasks);}

		/// Queues one task to be run by a worker thread.
		void	queue_task(std::function<void(void)> task)
		{
			std::unique_lock<std::mutex> lock(_queue_mutex);
			_tasks.emplace_back(task);
			_cv_task.notify_one();
		}
		/// Queues several tasks at once.
		void	queue_task(const std::vector<std::function<void(void)>> &tasks)
		{
			std::unique_lock<std::mutex> lock(_queue_mutex);
			for (auto &task : tasks)
				_tasks.emplace_back(task);
			_cv_task.notify_all();
		}

	private:
		void	_worker()
		{
			while (true)
			{
				std::unique_lock<std::mutex> latch(_queue_mutex);
				_cv_task.wait(latch, [this](){return (_stop || !_tasks.empty());});
				if (_stop)
					break ;
				else if (!_tasks.empty())
				{
					std::function<void(void)>	task = _tasks.front();
					_tasks.pop_front();
					_active_tasks++;

					latch.unlock();

					try
					{
						task();
					}
					catch(const std::exception &e)
					{
						(void)e;
					}

					latch.lock();
					_active_tasks--;
				}
			}
		}

		std::deque<std::function<void(void)>>					_tasks;
		std::atomic_int											_active_tasks;
		std::condition_variable 			_cv_task;
		std::mutex							_queue_mutex;

		std::vector<std::thread>			_worker_threads;

		std::atomic_bool					_stop;

		uint64_t							_threads_count = 0;
};

}}
