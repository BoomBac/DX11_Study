#include "Drawable.h"

void Drawable::Draw(Graphics& gfx)
{
	for (auto &i : binds)
	{
		i->Bind(gfx);
	}
	
}

void Drawable::AddBind(std::unique_ptr<Bindable > bind,Graphics& gfx)
{
	bind->Bind(gfx);
}
