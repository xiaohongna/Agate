#pragma once

#include "framework.h"

namespace agate
{
	class Window
	{
	public:
		Window();
		~Window();
		void Show(int nCmdShow);
	protected:
		LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam);

		void Paint();
		void OnResize(UINT width, UINT height);
	private:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		bool Register();
		void Create();
	private:
		HWND _handle;
		
	};
}

