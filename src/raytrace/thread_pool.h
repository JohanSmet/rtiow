// raytrace/thread_pool.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// a simple thread-pool abstraction (no promises/futures)

#pragma once

#include "types.h"

#include <cassert>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace rtiow {

class ThreadPool {
private:
	using mutex_t = std::mutex;
	using unique_lock_t = std::unique_lock<mutex_t>;
	using task_func_t = std::function<void()>;
	using condition_var_t = std::condition_variable;

public:
	// construction
	explicit ThreadPool(size_t num_workers) {
		assert(num_workers > 0);

		// create worker threads
		for (size_t i = 0; i < num_workers; ++i) {
			m_threads.emplace_back(&ThreadPool::thread_func, this);
		}
	}

	~ThreadPool() {
		// mark that threads should exit asap
		{
			unique_lock_t lock(m_tasks_mutex);
			m_should_stop = true;
		}

		// wake up all threads
		m_tasks_cv.notify_all();

		// wait for all threads to exit
		for (auto &thread: m_threads) {
			if (thread.joinable()) {
				thread.join();
			}
		}
	}

	ThreadPool(const ThreadPool &) = delete;
	ThreadPool(const ThreadPool &&) = delete;
	ThreadPool &operator=(const ThreadPool &) = delete;

	// information
	static int32_t hardware_concurrency() {
		return int32_t(std::thread::hardware_concurrency());
	}

	// task interface
	void add_task(task_func_t &&task) {
		{
			unique_lock_t lock(m_tasks_mutex);
			m_tasks.emplace_back(std::forward<task_func_t>(task));
		}

		// kick off a thread to execute this task
		m_tasks_cv.notify_one();
	}

private:
	void thread_func() {

		while (true) {
			task_func_t task;

			{
				// wait for tasks or changes in state flags
				unique_lock_t lock(m_tasks_mutex);
				m_tasks_cv.wait(lock, [this] {return m_should_stop || !m_tasks.empty();});

				// stop if asked unless there are still tasks to run
				if (m_should_stop && m_tasks.empty()) {
					return;
				}

				// fetch next task
				task = std::move(m_tasks.back());
				m_tasks.pop_back();
			}

			// execute tasks
			task();
		}
	}

private:
	std::vector<std::thread>	m_threads;
	bool						m_should_stop = false;

	std::vector<task_func_t>	m_tasks;
	mutex_t						m_tasks_mutex;
	condition_var_t				m_tasks_cv;

};


} // namespace rtiow
