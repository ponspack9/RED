#pragma once
#include "UIElement.h"

class Button : public UIElement
{
public:

	Button() {}
	Button(iPoint pos, SDL_Rect rect, UIType type);

	~Button() {}

	bool PreUpdate();
	bool PostUpdate();
};