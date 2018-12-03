#pragma once
#include "UIElement.h"

class Label :public UIElement
{
public:

	Label() {}
	Label(ActionType action, iPoint pos, UIType type, p2SString string);

	bool PreUpdate();
	bool PostUpdate();

	void Draw(SDL_Texture* sprites);

public:

	SDL_Texture* text;
};