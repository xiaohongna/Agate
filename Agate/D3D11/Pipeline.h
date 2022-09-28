#pragma once
#include <string>
#include "Renderer.h"
#include "VideoBuffer.h"

class PipelineBase
{
public:
	PipelineBase(PipelineType plt):_PiplineType(PipelineType::Color)
	{

	}

	virtual void Active(ID3D11DeviceContext* context);
	void UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t count);
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
	VideoBuffer<VertexXYColor, VideoBufferType::Vertex> _VertexBuffer;
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
protected:
	HRESULT CreateInputLayout(ID3D11Device* device, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength) override;
};

