#pragma once

#include <future>
#include <vector>
#include <queue>
#include <atomic>
#include <assert.h>
#include <Windows.h>

namespace Agate
{
	const UINT  WM_ASYNC = WM_USER + 1021;
	const UINT  WM_SYNC = WM_USER + 1022;

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
			DestroyWindow(message_window_handle);
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
		static inline void AsyncOnMain(Task&& task)
		{
			auto pTask = new std::function<void()>(std::move(task));
			::PostMessage(message_window_handle, WM_ASYNC, 0, (LPARAM)pTask);
		}
		/// <summary>
		/// 在主线程上执行任务，并返回结果
		/// </summary>
		/// <typeparam name="F">任务类型</typeparam>
		/// <param name="task"></param>
		/// <returns></returns>
		template<typename F>
		static auto SyncOnMain(F&& task)-> decltype(task())
		{
			using task_return_t = decltype(task());
			auto packTask = std::packaged_task<task_return_t()>(std::move(task));
			::SendMessage(message_window_handle, WM_SYNC, 0, (LPARAM)&packTask);
			return packTask.get();
		}
		
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

