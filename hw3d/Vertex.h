#pragma once
#include <map>
#include <vector>
#include <d3d11.h>

using namespace std;

enum class LayoutDes
{
	POSITION,
	NORMAL,
	TEXCOORD
};
//顶点结构
struct Vpostion
{
	float X;
	float Y;
	float Z;
};
struct Nomal
{
	float X;
	float Y;
	float Z;
	Nomal operator +=(const Nomal& input)
	{
		this->X += input.X;
		this->Y += input.Y;
		this->Z += input.Z;
		return *this;
	}
};

struct SimpleVertex
{
	Vpostion pos;
	Nomal nomal;
	struct TexCord
	{
		float u;
		float v;
	}Tex;
};


class VertexLayout
{
public:
	VertexLayout() ;
	~VertexLayout()=default;
	void AddInputLayout(LayoutDes);
	UINT GetArraySize();
	std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout();
private:
	std::map<LayoutDes, D3D11_INPUT_ELEMENT_DESC> m_InputDes;
	// 顶点布局对应偏移值
	std::map<LayoutDes, UINT> m_OffsetByte;
	std::vector<D3D11_INPUT_ELEMENT_DESC> v_InputDes;
	//字符对应枚举
	std::map<LPCSTR, LayoutDes> m_cmape;
};

VertexLayout::VertexLayout()
{
	D3D11_INPUT_ELEMENT_DESC postion { "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
	D3D11_INPUT_ELEMENT_DESC normal{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	D3D11_INPUT_ELEMENT_DESC texcoord{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 };
	m_InputDes.insert(pair<LayoutDes, D3D11_INPUT_ELEMENT_DESC>(LayoutDes::POSITION, postion));
	m_InputDes.insert(pair<LayoutDes, D3D11_INPUT_ELEMENT_DESC>(LayoutDes::NORMAL, normal));
	m_InputDes.insert(pair<LayoutDes, D3D11_INPUT_ELEMENT_DESC>(LayoutDes::TEXCOORD, texcoord));
	m_OffsetByte.insert(pair<LayoutDes, UINT>(LayoutDes::NORMAL, 12));
	m_OffsetByte.insert(pair<LayoutDes, UINT>(LayoutDes::POSITION, 12));
	m_OffsetByte.insert(pair<LayoutDes, UINT>(LayoutDes::TEXCOORD, 8));
	m_cmape.insert(pair<LPCSTR, LayoutDes>("NORMAL",LayoutDes::NORMAL));
	m_cmape.insert(pair<LPCSTR, LayoutDes>("POSITION",LayoutDes::POSITION));
	m_cmape.insert(pair<LPCSTR, LayoutDes>("TEXCOORD",LayoutDes::TEXCOORD));
}


void VertexLayout::AddInputLayout(LayoutDes ld)
{
	UINT offset = 0;
	for (auto i : v_InputDes)
	{
		offset += m_OffsetByte.find(m_cmape.find(i.SemanticName)->second)->second;
	}
	m_InputDes.find(ld)->second.AlignedByteOffset = offset;
	v_InputDes.push_back(m_InputDes.find(ld)->second);
}

UINT VertexLayout::GetArraySize()
{
	return v_InputDes.size();
}

std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout()
{
	return v_InputDes;
}
