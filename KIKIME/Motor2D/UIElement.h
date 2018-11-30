#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include "j1Module.h"
#include "j1App.h"
#include "j1Render.h"

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
	EXIT_GAME,
	SETTINGS,
	MAIN_MENU,
	NEXT_LEVEL,
	PREV_LEVEL,
	NO_ACTION
};

class UIElement
{
public:

	UIElement() {}
	UIElement(UIType type) { this->type = type; }

	virtual bool PreUpdate(float dt) { return true; }
	virtual bool PostUpdate(float dt) { return true; }

public:

	SDL_Rect	rect[IDLE + 1];
	iPoint		position;
	UIType		type;
	UIState		state;
	ActionType	action;

};







#endif //__UIELEMENT_H__
