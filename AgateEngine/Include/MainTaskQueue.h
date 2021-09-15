#pragma once

#include <queue>
#include <atomic>
#include <mutex>
#include <functional>
#include <Windows.h>

namespace Agate
{
const UINT  WM_ASYNC = WM_USER + 1021;
const UINT  WM_SYNC = WM_USER + 1022;

	class MainTaskQueue
	{
	public:
		MainTaskQueue();
		~MainTaskQueue();
		void Async(std::function<void()>&& task)
		{
			auto pTask = new std::function<void()>(std::move(task));
			::PostMessage(message_window_handle, WM_ASYNC,(WPARAM)this, (LPARAM)pTask);
		}

		void Sync(std::function<void()>& task)
		{
			::SendMessage(message_window_handle, WM_SYNC, (WPARAM)this, (LPARAM)&task);
		}
	private:
		HWND								message_window_handle;
	};
}

