#include "pch.h"
#include "Include/MainTaskQueue.h"

namespace Agate
{
    const LPCWSTR ClassName = L"QueueWindow";
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

	MainTaskQueue::MainTaskQueue()
	{
        RegisterQueueWndClass(0);
        message_window_handle = ::CreateWindowW(ClassName, NULL, WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, 0, nullptr);
	}

	MainTaskQueue::~MainTaskQueue()
	{
        DestroyWindow(message_window_handle);
	}
}