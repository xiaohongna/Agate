#pragma once

#include "framework.h"
#include "Agate2D.h"
namespace agate
{
	class Window: public Frame
	{
	public:
		Window();
		~Window();
		void Show(int nCmdShow);
	protected:
		LRESULT OnMessage(UINT message, WPARAM wParam, LPARAM lParam);

		void paint();
		void OnResize(UINT width, UINT height);
	private:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
		bool Register();
		void Create();
	private:
		HWND _handle;
		uint32_t _width;
		uint32_t _height;
		Share_Ptr<Canvas> _canvas;
		
	};
}

