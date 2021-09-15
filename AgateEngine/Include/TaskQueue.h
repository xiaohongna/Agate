#pragma once

#include <future>
#include <vector>
#include <queue>
#include <atomic>
#include <assert.h>
#include <Windows.h>

namespace Agate
{
	using Task = std::function<void()>;

	class TaskQueue final
	{
	public:
		~TaskQueue()
		{
			{
				std::lock_guard<std::mutex> lock(queue_mutex);
				stop = true;
			}
			condition.notify_all();
			for (std::thread& worker : workers)
				worker.join();
		}
		
		/// <summary>
		/// 在工程线程执行任务
		/// </summary>
		/// <param name="task"></param>
		static void Async(Task&& task)
		{
			{
				std::lock_guard<std::mutex> lock(queue_mutex);
				assert(stop == false);
				tasks.emplace(std::move(task));
			}
			condition.notify_one();
		}
		/// <summary>
		/// 在工作线程执行任务， then任务会同步到主线程调用
		/// </summary>
		/// <typeparam name="F"></typeparam>
		/// <param name="task">工作线程任务</param>
		/// <param name="then">主线程回调</param>
		template<typename F>
		static void Async(F&& task, std::function<void(decltype(task()))>&& then)
		{
			{
				std::lock_guard<std::mutex> lock(queue_mutex);
				assert(stop == false);
				tasks.emplace([tk = std::move(task), tn = std::move(then)]{
					auto result = tk();
					AsyncOnMain([result, t = std::move(tn)]{
						t(result);
					});
				});
			}
			condition.notify_one();
		}
	
		/// <summary>
		/// 在主线程上面异步执行任务
		/// </summary>
		/// <param name="task"></param>
		static inline void AsyncOnMain(Task&& task);

	private:
		TaskQueue();
	private:
		static TaskQueue							instance;
		static std::vector< std::thread >			workers;
		static std::queue< std::function<void()> >	tasks;
		static std::mutex queue_mutex;
		static std::condition_variable				condition;
		static bool									stop;

		static HWND									message_window_handle;
	};
}

