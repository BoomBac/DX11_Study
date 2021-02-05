#pragma once
#include "Bindable.h"
class Topology : Bindable
{
public:
	void Bind(Graphics& gfx) override;
};

void Topology::Bind(Graphics& gfx)
{
	GetContext(gfx)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
