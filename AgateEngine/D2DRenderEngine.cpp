#include "pch.h"
#include "D2DRenderEngine.h"
#include <assert.h>

namespace Agate
{
	HMODULE LoadLibraryFormSystem32(LPCWSTR lpLibFileName)
	{
		static int SupportSystem32Load = 0;
		if (SupportSystem32Load == 0)
		{
			//LOAD_LIBRARY_SEARCH_SYSTEM32 和 AddDllDirectory 是同时添加的，有AddDllDirectory，就可以使用LOAD_LIBRARY_SEARCH_SYSTEM32
			HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
			SupportSystem32Load = (hKernel32 != nullptr) && (GetProcAddress(hKernel32, "AddDllDirectory") != nullptr) ? 1 : 2;
		}

		if (SupportSystem32Load == 1)
		{
			return LoadLibraryEx(lpLibFileName, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		}
		else
		{
			return LoadLibrary(lpLibFileName);
		}
	}

	D2DRenderEngine::D2DRenderEngine(HWND wnd) :m_window(wnd), m_width(0), m_height(0)
	{
		if (SUCCEEDED(CoInitialize(NULL)))
		{
			D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
				m_pD2DFactory->QueryInterface(&m_pD2DFactory1);
				auto hr = CreateD3D11Device(D3D_DRIVER_TYPE_HARDWARE, D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS);
			if (SUCCEEDED(hr) && m_pD2DFactory1)
			{
				CComQIPtr<IDXGIDevice> pDxDevice = m_pD3DDevice11;
				hr = m_pD2DFactory1->CreateDevice(pDxDevice, &m_pD2DDevice);
			}
			CreateSwapChain();
			RECT rect{};
			GetWindowRect(m_window, &rect);
			Resize(rect.right - rect.left, rect.bottom - rect.top);
		}
	}

	bool D2DRenderEngine::Resize(int width, int height)
	{
		if (width == 0 || height == 0)
		{
			return true;
		}
		auto sz = m_pDeviceContext->GetPixelSize();
		if ((int)sz.width == width && (int)sz.height == height)
		{
			return true;
		}
		m_width = width;
		m_height = height;
		m_pDeviceContext->SetTarget(nullptr);
		HRESULT hr = m_SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		if (!SUCCEEDED(hr))
		{
			assert(false);
			return false;
		}
		CComPtr<IDXGISurface> pDXSurface;
		m_SwapChain->GetBuffer(0, IID_PPV_ARGS(&pDXSurface));
		if (!SUCCEEDED(hr))
		{
			assert(false);
			return false;
		}
		D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
			D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
			D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96.0f, 96.0f);

		CComPtr<ID2D1Bitmap1> pBitmap;
		hr = m_pDeviceContext->CreateBitmapFromDxgiSurface(pDXSurface, bitmapProperties, &pBitmap);
		if (!SUCCEEDED(hr))
		{
			assert(false);
			return false;
		}
		m_pDeviceContext->SetTarget(pBitmap);
		return true;
	}

	std::shared_ptr<RectangeVisual> D2DRenderEngine::CreateRectangeVisual()
	{
		return std::make_shared<RectangeVisual>();
	}

	void D2DRenderEngine::Render(RectangeVisual* root)
	{
		m_pDeviceContext->BeginDraw();
		D2D1_COLOR_F clearColor{ 1.0f, 1.0f,1.0f, 1.0f};
		m_pDeviceContext->Clear(clearColor);
		root->Render(this);
		m_pDeviceContext->EndDraw();
		m_SwapChain->Present(0, 0);
	}

	D2DRenderEngine::~D2DRenderEngine()
	{
		CoUninitialize();
	}

	void D2DRenderEngine::CreateSwapChain()
	{
		RECT rect{};
		GetWindowRect(m_window, &rect);

		HRESULT hr = m_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pDeviceContext);
		m_pRenderTarget = m_pDeviceContext;

		//UINT      m4xMsaaQuality;   // MSAA支持的质量等级
		//m_D2DFactory->m_pD3DDevice11->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m4xMsaaQuality);
		//assert(m4xMsaaQuality > 0);
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
		swapChainDesc.Width = rect.right - rect.left;
		swapChainDesc.Height = rect.bottom - rect.top;
		swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;          //DXGI_SCALING_NONE;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;  //  DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = 0;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

		CComQIPtr<IDXGIDevice> pDxDevice = m_pD3DDevice11;
		CComPtr<IDXGIAdapter> pDxAdapter;
		pDxDevice->GetAdapter(&pDxAdapter);
		CComPtr<IDXGIFactory2> pFactory;
		pDxAdapter->GetParent(IID_PPV_ARGS(&pFactory));

		hr = pFactory->CreateSwapChainForHwnd(pDxDevice, m_window, &swapChainDesc, nullptr, nullptr, &m_SwapChain);
		if (!SUCCEEDED(hr))
		{
			assert(true);
			return;
		}
		DXGI_RGBA argb = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_SwapChain->SetBackgroundColor(&argb);
	}


	HRESULT D2DRenderEngine::CreateD3D11Device(D3D_DRIVER_TYPE driverType, UINT flags)
	{
		const D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0
		};
		D3D_FEATURE_LEVEL   d3dLevel{};
		IDXGIAdapter*  pAdapter = NULL;
		driverType = pAdapter == NULL ? D3D_DRIVER_TYPE_HARDWARE : D3D_DRIVER_TYPE_UNKNOWN;
		//flags = flags | D3D11_CREATE_DEVICE_DEBUG;
		auto hr = D3D11CreateDevice(pAdapter, driverType, 0, flags, featureLevels,
			ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &m_pD3DDevice11, &d3dLevel, &m_pD3DDeviceContext11);
		assert(SUCCEEDED(hr));
		return hr;
	}
}
