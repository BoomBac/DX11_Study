#pragma once
#include "Graphics.h"
#include "Bindable.h"
#include <vector>
#include <memory>
class Drawable
{
public:
	Drawable() = default;
	void Draw(Graphics& gfx);
	void AddBind(std::unique_ptr<Bindable > bind, Graphics& gfx);
private:
	std::vector<std::unique_ptr<Bindable>> binds;
};

