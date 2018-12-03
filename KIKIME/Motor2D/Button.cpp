#include "Button.h"

Button::Button(ActionType action, iPoint pos, SDL_Rect rect, UIType type) : UIElement(type)
{
	this->initial_pos = pos;
	this->position = pos;
	this->rect[IDLE] = rect;
	this->type = type;
	this->state = IDLE;

	this->rect[HOVER] = SDL_Rect({ 0,111,228,68 });
	this->rect[CLICK_DOWN] = SDL_Rect({ 412,167,228,68 });
	this->rect[CLICK_UP] = this->rect[HOVER];

	this->action = action;
}

bool Button::PreUpdate()
{
	position.x = initial_pos.x - App->render->camera.x;
	position.y = initial_pos.y - App->render->camera.y;

	LOG("button pos: %d - %d", position.x, position.y);

	return true;
}

bool Button::PostUpdate()
{
	return true;
}
