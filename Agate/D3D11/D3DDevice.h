#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <memory>
#include <map>
#include "Drawable.h"
#include "Renderer.h"
#include "VideoBuffer.h"
#include "Pipeline.h"
#include "Brush.h"
#include "AssetManager.h"

namespace agate
{

	struct D3DTexture
	{
		uint32_t    width;
		uint32_t    height;
		CComPtr<ID3D11Texture2D> resource;
		CComPtr<ID3D11ShaderResourceView> resourceView;
		CComPtr<ID3D11RenderTargetView> renderTargetView;
	};

	class  D3DDevice: public IAssetManagerDelegate 
	{
	public:
		D3DDevice() :
			_CurRenderTarget{},
			_CurrentPipline{},
			_CurrentBlend{ BlendMode::Blend }
		{

		}

		HRESULT CreateSwapChain(HWND hwnd, IDXGISwapChain** swapChain);

		HRESULT CreateRenderTarget(IDXGISwapChain* swapChain, ID3D11RenderTargetView** rootView);

		void ResetState();

		void SetBlend(BlendMode blend);

		void SetRenderTarget(ID3D11RenderTargetView* targetView, uint32_t width, uint32_t height);

		void Clear(Color color);

		void Draw(const BatchDrawData& data);

		void Dispose();
#pragma region IAssetManagerDelegate
		const AssetManagerConfig& GetConfig() override;

		TextureHandle CreateRenderTarget(uint32_t width, uint32_t height) override;

		TextureHandle CreateTexture(uint32_t width, uint32_t height, void* sysMem) override;

		void ReleaseTexture(TextureHandle handle) override; 
#pragma endregion
	private:
		HRESULT CreateOther();
		HRESULT CreateBlendState();
		HRESULT CreateSamplerState();
	private:
		CComPtr<IDXGIFactory>           _Factory;
		CComPtr<ID3D11Device>           _Device;
		CComPtr<ID3D11DeviceContext>    _DeviceContext;
		ID3D11RenderTargetView*			_CurRenderTarget;

		VideoBuffer<float, VideoBufferType::Constant>  _VertexConstantBuffer;

		CComPtr<ID3D11RasterizerState>  _RasterizerState;
		CComPtr<ID3D11DepthStencilState>  _DepthStencilState;

		CComPtr<ID3D11BlendState>       _BlendStates[5];
		CComPtr<ID3D11SamplerState>     _Samplers[(ULONG)SamplerMode::SamplerModeCount];
		std::unique_ptr<PipelineBase>    _Piplines[2];

		PipelineBase* _CurrentPipline;
		BlendMode   _CurrentBlend;
	};
}
