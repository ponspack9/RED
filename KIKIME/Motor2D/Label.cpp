#include "Label.h"

Label::Label(ActionType action, iPoint pos, UIType type, p2SString string) : UIElement(type)
{
	this->initial_pos = pos;
	this->position = pos;
	this->type = type;
	this->state = IDLE;

	this->rect[IDLE].x = position.x;
	this->rect[IDLE].y = position.y;

	this->string = string;
	this->action = action;

	text = App->font->Print(string.GetString(), { 255,255,255,255 }, App->font->default);
	App->font->CalcSize(string.GetString(), rect[IDLE].w, rect[IDLE].h, App->font->default);
}

bool Label::PreUpdate()
{
	if (action == GAME_TIMER)
	{
		// Use change text function
		char game_time[50];
		sprintf(game_time, "%.3f", App->GetTimerReadSec());

		ChangeText(game_time);
	}
	if (action == SCORE)
	{
		//-------------------------//
	}
	float val = 1.1;

	position.x = initial_pos.x - App->render->camera.x;
	position.y = initial_pos.y - App->render->camera.y;

	return true;
}

bool Label::PostUpdate()
{
	return true;
}

void Label::Draw(SDL_Texture* sprites)
{
	sprites = text;
	App->render->Blit(sprites, position.x, position.y);
}

void Label::ChangeText(const char * string)
{
	App->tex->UnLoad(text);

	text = App->font->Print(string, { 255,255,255,255 }, App->font->default);
	App->font->CalcSize(string, rect[state].w, rect[state].h, App->font->default);

	this->string = string;	
}

void Label::HandleAction() 
{
	if (action != INFO)
	{
		switch (state)
		{
		case HOVER:
			ChangeText("HOVER");
			break;
		case IDLE:
			ChangeText("IDLE");
			break;
		case CLICK_DOWN:
			ChangeText("CLICKASU BRO");
			break;
		}
	}
}
