
#include "TaskQueue.h"

namespace Agate
{
    const LPCWSTR ClassName = L"QueueWindow";

    TaskQueue TaskQueue::instance;
    std::vector< std::thread > TaskQueue::workers;
    std::queue< std::function<void()> > TaskQueue::tasks;

    std::mutex TaskQueue::queue_mutex;
    std::condition_variable TaskQueue::condition;
    bool TaskQueue::stop = false;
    HWND TaskQueue::message_window_handle;

    
    LRESULT CALLBACK QueueWndWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_ASYNC:
        {
            auto pTask = reinterpret_cast<std::function<void()>*>(lParam);
            (*pTask)();
            delete pTask;
        }
        break;
        case WM_SYNC:
        {
            auto pTask = reinterpret_cast<std::function<void()>*>(lParam);
            (*pTask)();
        }
        break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    ATOM RegisterQueueWndClass(HINSTANCE hInstance)
    {
        WNDCLASSEXW wcex{};

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = QueueWndWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszClassName = ClassName;
        return RegisterClassExW(&wcex);
    }

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
        RegisterQueueWndClass(0);
        message_window_handle = ::CreateWindowW(ClassName, NULL, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, 0, nullptr);
	}
}