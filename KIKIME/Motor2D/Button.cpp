#include "Button.h"

Button::Button(iPoint pos, SDL_Rect rect, UIType type) : UIElement(type)
{
	this->position = pos;
	this->rect[IDLE] = rect;
	this->type = type;
	this->state = IDLE;

	this->rect[HOVER] = SDL_Rect({ 6,117,217,56 });
	this->rect[CLICK_DOWN] = SDL_Rect({ 417,173,217,56 });
	this->rect[CLICK_UP] = this->rect[HOVER];
}

bool Button::PreUpdate()
{
	position.x = App->render->viewport.w / 12 - App->render->camera.x;
	position.y = App->render->viewport.w / 12 - App->render->camera.y;

	LOG("button pos: %d - %d", position.x, position.y);

	return true;
}

bool Button::PostUpdate()
{
	return true;
}
