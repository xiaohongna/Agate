// AgateDemo.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "AgateDemo.h"
#include "Window.h"

// 全局变量:
HINSTANCE hInst;                                // 当前实例

 agate::Share_Ptr<agate::Window> window;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    window = new agate::Window();
    window->Show(SW_SHOW);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_AGATEDEMO));
    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            if (msg.message == WM_QUIT)
            {
                window = nullptr;
                return 0;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
