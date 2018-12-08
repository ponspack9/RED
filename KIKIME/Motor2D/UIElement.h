#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include "j1Module.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1EntityManager.h"

enum UIType 
{ 
	BUTTON,
	LABEL, 
	IMAGE, 
	NONE 
};

enum UIState
{
	HOVER,
	CLICK_DOWN,
	CLICK_UP,
	IDLE
};

enum ActionType
{
	MAIN_MENU,
	SETTINGS,
	EXIT_GAME,
	PLAY_PAUSE,
	SCORE,
	LIFE_SYSTEM,
	GAME_TIMER,
	PLAYER_NAME,
	INFO,
	NO_ACTION
};

class UIElement
{
public:

	UIElement() {}
	UIElement(UIType type) { this->type = type; }

	virtual bool PreUpdate() { return true; }
	virtual bool PostUpdate() { return true; }

	virtual void Draw(SDL_Texture* sprites = nullptr)
	{
		App->render->Blit(sprites, position.x, position.y, &rect[state]);
	}

	virtual void HandleAction() {}

public:

	SDL_Rect	rect[IDLE + 1];
	iPoint		position;
	UIType		type;
	UIState		state;
	ActionType	action;
	p2SString	string;
	iPoint		initial_pos;

	j1Module*	callback;
};
#endif //__UIELEMENT_H__
