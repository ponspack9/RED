#include "Image.h"

Image::Image(iPoint pos, SDL_Rect rect, UIType type, UIElement* parent, bool visible) : UIElement(type, parent, visible)
{
	this->initial_pos = pos;
	this->rect[IDLE] = rect;
	this->rect[HOVER] = rect;
	this->rect[CLICK_DOWN] = rect;
	this->rect[CLICK_UP] = rect;

	this->position = pos;
	this->type = type;
	
	this->state = IDLE;
}


