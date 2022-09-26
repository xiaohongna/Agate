#pragma once
#include <string>
#include "Renderer.h"
#include "VideoBuffer.h"

class PiplineBase
{
public:
	bool Load(ID3D11Device* device, const std::wstring& vsfile, const std::wstring& psfile)
	{
		return LoadVertexShader(device, vsfile) && LoadPixelShader(device, psfile);
	}
	virtual void Active(ID3D11DeviceContext* context);
	void UpdateVertex(ID3D11DeviceContext* context, byte* data, uint32_t size);
	void UpdateIndex(ID3D11DeviceContext* context, DrawIndex* data, uint32_t size);
protected:
	bool LoadVertexShader(ID3D11Device* device, const std::wstring& csofile);
	bool LoadPixelShader(ID3D11Device* device, const std::wstring& csofile);
protected:
	VideoBuffer<byte, VideoBufferType::Vertex> _VertexBuffer;
	VideoBuffer<DrawIndex, VideoBufferType::VertexIndex> _IndexBuffer;
	CComPtr<ID3D11VertexShader> _VertexShader;
	CComPtr<ID3D11InputLayout> _InputLayout;
	CComPtr<ID3D11PixelShader> _PixelShader;
};

