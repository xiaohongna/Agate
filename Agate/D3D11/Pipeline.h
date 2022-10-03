#pragma once
#include <string>
#include "Renderer.h"
#include "VideoBuffer.h"

class PipelineBase
{
public:
	PipelineBase(PipelineType plt) :_PiplineType{plt}
	{

	}

	virtual void Active(ID3D11DeviceContext* context);
	virtual void UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t count) = 0; 
	void UpdateIndex(ID3D11DeviceContext* context, DrawIndex* data, uint32_t count);
	PipelineType GetType()
	{
		return _PiplineType;
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
	PipelineType		_PiplineType;
};

class ColorPipline: public PipelineBase
{
public:
	ColorPipline() : PipelineBase(PipelineType::Color)
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
	TextureColorPipline() : PipelineBase(PipelineType::TextureColor)
	{

	}
	bool Load(ID3D11Device* device);
protected:
	HRESULT CreateInputLayout(ID3D11Device* device, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength) override;
	VideoBuffer<VertexXYUVColor, VideoBufferType::Vertex> _VertexBuffer;
};
