#include "Button.h"

Button::Button()
{
	initial_pos = { 0,0 };
	position    = { 0,0 };

	state	    = IDLE;
			    
	type	    = UIType::BUTTON;
	action      = NO_ACTION;
	parent      = nullptr;
	callback    = nullptr;
	visible     = true;
	color       = GREY;
	movable     = false;
	is_moving   = false;
}

Button::Button(iPoint pos,const Button & b,ActionType action, UIElement* parent) : UIElement(b.type, parent, b.visible)
{
	this->initial_pos = pos;
	this->position = pos;

	this->state = IDLE;
	this->action = action;

	this->callback = b.callback;

	this->rect[IDLE]		= b.rect[IDLE];
	this->rect[HOVER]		= b.rect[HOVER];
	this->rect[CLICK_DOWN]	= b.rect[CLICK_DOWN];
	this->rect[CLICK_UP]	= b.rect[CLICK_UP];

	this->color = b.color;
}

void Button::HandleAction()
{
	switch (action)
	{
	case MAIN_MENU:
		App->GoToMainMenu();
		break;
	case SETTINGS:
		App->ShowSettings();
		break;
	case PAUSE:
		App->TogglePause();
		App->ShowInGamePause();
		break;
	case CONTINUE:
		App->LoadGame();
		break;
	case EXIT_GAME:
		App->Exit();
		break;
	case CREDITS:
		App->ShowCredits();
		break;
	case WEBSITE:
		ShellExecuteA(NULL, "open", "www.google.com", NULL, NULL, SW_SHOWNORMAL);
		break;
	case START:
		App->RestartGame();
	}
}
