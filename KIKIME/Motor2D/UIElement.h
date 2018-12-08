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
	UIElement(UIType type, UIElement* parent, bool visible) { 
		this->type = type; 
		this->parent = parent;
		this->visible = visible;
	}

	virtual bool PreUpdate() { return true; }
	virtual bool PostUpdate() { 
		if (parent != nullptr)
		{
			visible = parent->visible;
			if (action != PLAYER_NAME)
			{
				position.x = parent->position.x + initial_pos.x;
				position.y = parent->position.y + initial_pos.y;
			}
		}
		return true; 
	}

	virtual void Draw(SDL_Texture* sprites = nullptr)
	{
		App->render->Blit(sprites, position.x, position.y, &rect[state], 0);
	}

	int CenterX(int offset = 0) {
		if (parent != nullptr)
			initial_pos.x = ((parent->rect[IDLE].w /2) - rect[IDLE].w/2) + offset;
		return initial_pos.x;
	}
	int CenterY(int offset = 0) {
		if (parent != nullptr)
			initial_pos.y = ((parent->rect[IDLE].h / 2) - rect[IDLE].h / 2) + offset;
		return initial_pos.y;
	}

	void Center(int offsetX = 0, int offsetY = 0) {
		if (parent != nullptr) {
			initial_pos.x = ((parent->rect[IDLE].w / 2) - rect[IDLE].w / 2) + offsetX;
			initial_pos.y = ((parent->rect[IDLE].h / 2) - rect[IDLE].h / 2) + offsetY;
		}
	}

	virtual void HandleAction() {}

public:

	SDL_Rect	rect[IDLE + 1];

	iPoint		position;
	iPoint		initial_pos;

	UIType		type;
	UIState		state;
	ActionType	action;

	p2SString	string;

	UIElement*	parent;

	j1Module*	callback;

	bool		visible;
};
#endif //__UIELEMENT_H__
