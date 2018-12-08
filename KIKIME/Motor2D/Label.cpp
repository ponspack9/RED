#include "Label.h"

Label::Label(ActionType action, iPoint pos, UIType type, p2SString string, UIElement* parent, bool visible) : UIElement(type, parent,visible)
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
		char game_time[50];
		sprintf(game_time, "%.3f", App->GetTimerReadSec());

		ChangeText(game_time);
	}
	if (action == SCORE)
	{
		//-------------------------//
	}
	if (action == PLAYER_NAME)
	{
		position.x = App->entitymanager->player_ref->position.x + App->entitymanager->player_ref->rect.w / 2 - rect[IDLE].w / 2;
		position.y = App->entitymanager->player_ref->position.y - 20;

		return true;
	}

	return true;
}

bool Label::PostUpdate()
{
	return true;
}

//void Label::Draw(SDL_Texture* sprites)
//{
//	sprites = text;
//	App->render->Blit(text, position.x, position.y);
//}

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
	  	//ChangeText("HOVER");
		  break;
	  case IDLE:
		  //ChangeText("IDLE");
		  break;
	  case CLICK_DOWN:
		  //ChangeText("CLICKASU BRO");
		  break;
	}
}
