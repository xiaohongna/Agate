#pragma once
#include <Windows.h>
#include "DrawingContext.h"

namespace agate
{
	DrawingContext* CreateDrawContext(HWND hwnd);
	void ClearResource();
}

