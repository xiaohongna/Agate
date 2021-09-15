#include "pch.h"

#include "../Include/TaskQueue.h"

namespace Agate
{
    const UINT  WM_ASYNC = WM_USER + 1021;
    const UINT  WM_SYNC = WM_USER + 1022;

    TaskQueue TaskQueue::instance;
    std::vector< std::thread > TaskQueue::workers;
    std::queue< std::function<void()> > TaskQueue::tasks;

    std::mutex TaskQueue::queue_mutex;
    std::condition_variable TaskQueue::condition;
    bool TaskQueue::stop = false;
    HWND TaskQueue::message_window_handle;


	TaskQueue::TaskQueue()
	{
        unsigned int threads = std::thread::hardware_concurrency() - 2;
        for (unsigned int i = 0; i < threads; ++i)
            workers.emplace_back(
                [this]
                {
                    while (true)
                    {
                        std::function<void()> task;
                        {
                            std::unique_lock<std::mutex> lock(this->queue_mutex);
                            this->condition.wait(lock,
                                [this] { return this->stop || !this->tasks.empty(); });
                            if (this->stop && this->tasks.empty())
                                return;
                            task = std::move(this->tasks.front());
                            this->tasks.pop();
                        }

                        task();
                    }
                }
                );
	}

    inline void TaskQueue::AsyncOnMain(Task&& task)
    {

    }
}