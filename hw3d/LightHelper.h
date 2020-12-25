#pragma once
#include <DirectXMath.h>

using namespace DirectX;

struct DirectionalLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse; 
	XMFLOAT4 Specular; 
	XMFLOAT3 Direction;
};

struct PointLight
{
	XMFLOAT4 Ambient; 
	XMFLOAT4 Diffuse; 
	XMFLOAT4 Specular;
	XMFLOAT3 Position; 
	float Range;
	XMFLOAT3 Att;
};

struct Material
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;
};