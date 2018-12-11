#ifndef __UIELEMENT_H__
#define __UIELEMENT_H__

#include "j1Module.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Fonts.h"
#include "j1EntityManager.h"


enum UIColor {
	BLUE,
	RED,
	YELLOW,
	GREY,
	GREEN
};
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
	START,
	CONTINUE,
	MAIN_MENU,
	SETTINGS,
	CHANGE_VOLUME,
	EXIT_GAME,
	CREDITS,
	WEBSITE,
	PAUSE,
	SCORE,
	LIFE_SYSTEM,
	GAME_TIMER_SECS,
	GAME_TIMER_MINS,
	PLAYER_NAME,
	INFO,
	BLUE_COUNTER,
	GREEN_COUNTER,
	LAST_DEATH,

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
		movable = true;
	}

	virtual bool PreUpdate() { return true; }
	virtual bool PostUpdate() { 
		if (action == CHANGE_VOLUME)
		{
			LOG("INITIAL_POS POSTUPDATE: %d,%d", initial_pos.x, initial_pos.y);
			LOG("POSITION    POSTUPDATE: %d,%d", position.x, position.y);
			LOG("PARENT      POSTUPDATE: %d,%d", parent->position.x, parent->position.y);
		}

		if (parent != nullptr)
		{
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

	void SetInvisible() {
		p2List_item<UIElement*>* item = sons.start;
		visible = false;
		for (item; item != nullptr; item = item->next) {
			item->data->SetInvisible();
		}
	}

	void SetVisible() {
		p2List_item<UIElement*>* item = sons.start;
		visible = true;
		for (item; item != nullptr; item = item->next) {
			item->data->SetVisible();
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
	bool		movable;

	p2List<UIElement*>	sons;
};
#endif //__UIELEMENT_H__
