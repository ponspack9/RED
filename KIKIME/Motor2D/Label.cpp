#include "Label.h"
#include "j1Textures.h"

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
		// Use change text function
		/*const char game_time = App->GetTimerReadSec();
		text = App->font->Print(&game_time, { 255,255,255,255 }, App->font->default);
		App->font->CalcSize(&game_time, rect[IDLE].w, rect[IDLE].h, App->font->default);*/
	}
	if (action == SCORE)
	{
		//-------------------------//
	}

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
