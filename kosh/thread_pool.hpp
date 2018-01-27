#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

namespace kosh
{
	class thread_pool
	{
	public:
		explicit thread_pool(std::size_t = std::max(1u,
													std::thread::hardware_concurrency()));

		template <class F, class... Args>
		std::future<std::result_of_t<F(Args...)>> add_task(F&& f, Args&&... args);

		~thread_pool();

	private:
		std::vector<std::thread> workers_;

		std::queue<std::function<void()>> tasks_;

		std::mutex queue_mutex_;
		std::condition_variable condition_;
		bool stop_;
	};

	inline thread_pool::thread_pool(std::size_t threads) : stop_(false)
	{
		for (std::size_t i = 0; i < threads; ++i)
		{
			workers_.emplace_back([this] {
				for (;;)
				{
					std::function<void()> task;

					{
						std::unique_lock<std::mutex> lock(queue_mutex_);
						condition_.wait(lock,
										[this] { return stop_ || !tasks_.empty(); });

						if (stop_ && tasks_.empty())
							return;

						task = std::move(tasks_.front());
						tasks_.pop();
					}

					task();
				}
			});
		}
	}

	template <class F, class... Args>
	std::future<std::result_of_t<F(Args...)>> thread_pool::add_task(F&& f, Args&&... args)
	{
		using return_type = std::result_of_t<F(Args...)>;

		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...));

		std::future<return_type> res = task->get_future();

		{
			std::lock_guard<std::mutex> lock(queue_mutex_);

			if (stop_)
				throw std::runtime_error("enqueue on stopped ThreadPool");

			tasks_.emplace([task = std::move(task)]() { (*task)(); });
			condition_.notify_one();
		}

		return res;
	}

	inline thread_pool::~thread_pool()
	{
		{
			std::lock_guard<std::mutex> lock(queue_mutex_);
			stop_ = true;
			condition_.notify_all();
		}

		for (std::thread& worker : workers_) worker.join();
	}

}  // namespace kosh

#endif
