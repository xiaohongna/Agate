#pragma once
#include <Windows.h>
#include "graphic/DrawingContext.h"

namespace agate
{
	DrawingContext* CreateDrawContext(HWND hwnd);
	void ClearResource();
}

