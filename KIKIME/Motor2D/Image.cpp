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
	position.x = App->render->viewport.w / 2 - App->render->camera.x;
	position.y = App->render->viewport.h / 12 - App->render->camera.y + 25;

	return true;
}

bool Image::PostUpdate()
{
	return true;
}
