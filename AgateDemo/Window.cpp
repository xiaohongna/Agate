#include "Window.h"
#include <assert.h>
namespace agate
{
	auto szWindowClass = L"agatewindow";

	Window::Window() :_handle{}
	{
	}
	Window::~Window()
	{


	}
	void Window::Show(int nCmdShow)
	{
		if (_handle == 0)
		{
			Register();
			Create();
			assert(_handle != 0);
		}
		::ShowWindow(_handle, nCmdShow);
	}

	LRESULT Window::OnMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(_handle, &ps);
			paint();
			EndPaint(_handle, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_SIZE:
			OnResize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
			break;
		default:
			return ::DefWindowProc(_handle, message, wParam, lParam);
		}
		return 0;
	}

	void Window::paint()
	{
		if (_canvas) 
		{
			_canvas->beginPaint(_width, _height);
			draw(_canvas);
			_canvas->endPaint();
		}
	}

	void Window::OnResize(UINT width, UINT height)
	{
		_width = width;
		_height = height;
	}

	LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		auto* wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		if (wnd)
		{
			return wnd->OnMessage(message, wParam, lParam);
		}
		else
		{
			return ::DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	bool Window::Register()
	{
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = 0;
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = 0;
		return RegisterClassExW(&wcex) != 0;
	}

	void Window::Create()
	{
		HWND hWnd = CreateWindowW(szWindowClass, L"Agate", WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, 0, nullptr);
		if (hWnd)
		{
			_handle = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
		}
		_canvas = new Canvas();
		_canvas->bindContext(this);
	}

}