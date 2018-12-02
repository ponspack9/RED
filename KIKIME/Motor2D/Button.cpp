#include "Button.h"

Button::Button(ActionType action, iPoint pos, SDL_Rect rect, UIType type) : UIElement(type)
{
	this->position = pos;
	this->rect[IDLE] = rect;
	//this->type = type;
	this->state = IDLE;

	this->rect[HOVER] = SDL_Rect({ 0,111,228,68 });
	this->rect[CLICK_DOWN] = SDL_Rect({ 412,167,228,68 });
	this->rect[CLICK_UP] = this->rect[HOVER];

	this->action = action;
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
