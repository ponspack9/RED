#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include "j1Module.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Fonts.h"

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
	GAME_TIMER,
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
		if (parent != nullptr){
			visible &= parent->visible;
			position.x = parent->position.x + initial_pos.x;
			position.y = parent->position.y + initial_pos.y;
		}
		return true; 
	}

	virtual void Draw(SDL_Texture* sprites = nullptr)
	{
		App->render->Blit(sprites, position.x, position.y, &rect[state], 0);
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
