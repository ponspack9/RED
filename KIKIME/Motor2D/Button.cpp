#include "Button.h"

Button::Button(ActionType action, iPoint pos, SDL_Rect rect, UIType type, j1Module* callback, UIElement* parent, bool visible) : UIElement(type, parent, visible)
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
	this->callback = callback;
}

void Button::HandleAction()
{
	switch (action)
	{
	case MAIN_MENU:
		App->GoToMainMenu();
		break;
	case SETTINGS:
		break;
	case PLAY_PAUSE:
		// Need to have only one parent above him to work properly
		App->TogglePause();
		if (parent) parent->visible = false;
		break;
	case EXIT_GAME:
		App->Exit();
		break;
	}
}
