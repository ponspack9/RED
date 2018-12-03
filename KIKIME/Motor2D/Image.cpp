#include "Image.h"

Image::Image(iPoint pos, SDL_Rect rect, UIType type) : UIElement(type)
{
	this->initial_pos = pos;
	this->rect[IDLE] = rect;
	this->position = pos;
	this->type = type;
	
	this->state = IDLE;
}

bool Image::PreUpdate()
{
	position.x = initial_pos.x - App->render->camera.x;
	position.y = initial_pos.y - App->render->camera.y;

	return true;
}

bool Image::PostUpdate()
{
	return true;
}
