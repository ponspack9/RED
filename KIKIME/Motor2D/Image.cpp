#include "Image.h"

Image::Image(iPoint pos, SDL_Rect rect, UIType type) : UIElement(type)
{
	this->rect[IDLE] = rect;
	this->position = pos;
	this->type = type;
	
	this->state = IDLE;
}

bool Image::PreUpdate()
{
	return true;
}

bool Image::PostUpdate()
{
	return true;
}
