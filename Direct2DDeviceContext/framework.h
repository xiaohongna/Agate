// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//DX Header Files
#include <d3d11.h>
#include "Direct2DDeviceContext.h"
#include <d2d1.h>
#include<d2d1_1.h>
#include<d2d1_2.h>
#include<d2d1_1helper.h>
#include<dxgi1_2.h>
#include <atlbase.h>

//Unavailable 
#define SAFERELEASE(ppInterfaceToRelease) if(ppInterfaceToRelease){ppInterfaceToRelease->Release() ; ppInterfaceToRelease = NULL ;}

namespace DX
{
	// Convert DirectX error codes to exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			// Set a breakpoint on this line to catch DX API errors.
			throw hr;
			//need Windows 8+ OS
			//throw Platform::Exception::CreateException(hr);
		}
	}

#if defined(_DEBUG)
	// Check for SDK Layer support.
	inline bool SdkLayersAvailable()
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
			0,
			D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
			nullptr,                    // Any feature level will do.
			0,
			D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			nullptr,                    // No need to keep the D3D device reference.
			nullptr,                    // No need to know the feature level.
			nullptr                     // No need to keep the D3D device context reference.
		);

		return SUCCEEDED(hr);
	}
#endif
}
