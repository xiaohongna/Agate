#pragma once
#include <string>
#include "Agate2D.h"
#include "VideoBuffer.h"

namespace agate
{

	class PipelineBase
	{
	public:
		PipelineBase(EffectType plt) :_PiplineType{ plt }
		{

		}

		virtual void Active(ID3D11DeviceContext* context);
		virtual void UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t count) = 0;
		void UpdateIndex(ID3D11DeviceContext* context, DrawIndex* data, uint32_t count);
		EffectType GetType()
		{
			return _PiplineType;
		}
		virtual void Dispose()
		{
			_IndexBuffer.Dispose();
			_VertexShader.Release();
			_InputLayout.Release();
			_PixelShader.Release();
		}
	protected:
		virtual HRESULT CreateInputLayout(ID3D11Device* device, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength) = 0;
		bool LoadVertexShader(ID3D11Device* device, const std::wstring& csofile);
		bool LoadPixelShader(ID3D11Device* device, const std::wstring& csofile);
	protected:
		VideoBuffer<DrawIndex, VideoBufferType::VertexIndex> _IndexBuffer;
		CComPtr<ID3D11VertexShader> _VertexShader;
		CComPtr<ID3D11InputLayout> _InputLayout;
		CComPtr<ID3D11PixelShader> _PixelShader;
		EffectType		_PiplineType;
	};

	class ColorPipline : public PipelineBase
	{
	public:
		ColorPipline() : PipelineBase(EffectType::Color)
		{

		}
		bool Load(ID3D11Device* device);
		void Active(ID3D11DeviceContext* context) override;
		void UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t count) override;
	protected:
		HRESULT CreateInputLayout(ID3D11Device* device, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength) override;
		VideoBuffer<VertexXYColor, VideoBufferType::Vertex> _VertexBuffer;
	};

	class TextureColorPipline : public PipelineBase
	{
	public:
		void Active(ID3D11DeviceContext* context) override;
		void UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t count) override;
		TextureColorPipline() : PipelineBase(EffectType::TextureColor)
		{

		}
		bool Load(ID3D11Device* device);
		void Dispose() override
		{
			PipelineBase::Dispose();
			_VertexBuffer.Dispose();
		}
	protected:
		HRESULT CreateInputLayout(ID3D11Device* device, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength) override;
		VideoBuffer<VertexXYUVColor, VideoBufferType::Vertex> _VertexBuffer;
	};

	class DisplacementPipline : public PipelineBase
	{
	public:
		DisplacementPipline() : PipelineBase(EffectType::DistortTextureColor)
		{

		}
		void Active(ID3D11DeviceContext* context) override;
		void UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t count) override;
		bool Load(ID3D11Device* device);
		void Dispose() override
		{
			PipelineBase::Dispose();
			_VertexBuffer.Dispose();
		}
	protected:
		HRESULT CreateInputLayout(ID3D11Device* device, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength) override;
		VideoBuffer<VertexXYUVColor, VideoBufferType::Vertex> _VertexBuffer;
	};
}