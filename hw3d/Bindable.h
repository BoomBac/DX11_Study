#pragma once
#include "Graphics.h"

class Bindable
{
public:
	virtual void Bind(Graphics& gfx) = 0;
	virtual ~Bindable()=default;

protected:
	// 将基类获取的graphics成员暴露给子类
	static ID3D11DeviceContext* GetContext(Graphics& gfx);
	static ID3D11Device* GetDevice(Graphics& gfx);
};
