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

	text = App->font->Print(string.GetString(), { 0,0,0,255 }, App->font->default);
	App->font->CalcSize(string.GetString(), rect[IDLE].w, rect[IDLE].h, App->font->default);

	this->rect[HOVER] = rect[IDLE];
	this->rect[CLICK_DOWN] = rect[IDLE];
	this->rect[CLICK_UP] = rect[IDLE];
}

bool Label::PreUpdate()
{
	if (action == GAME_TIMER_SECS)
	{
		char game_time_secs[50];
		if (App->GetTimerReadSec() >= 1)
		{
			App->in_game_timer.sec++;
			App->game_timer.Start();
		}
		sprintf_s(game_time_secs, "%d", App->in_game_timer.sec);

		ChangeText(game_time_secs);
	}
	if (action == GAME_TIMER_MINS)
	{
		char game_time_mins[50];
		if (App->in_game_timer.sec == 60)
		{
			App->in_game_timer.min++;
			App->in_game_timer.sec = 0;
		}
		sprintf_s(game_time_mins, "%d", App->in_game_timer.min);

		ChangeText(game_time_mins);
	}
	if (action == SCORE)
	{
		char curr_score[50];
		sprintf_s(curr_score, "%d", App->entitymanager->score);
		
		ChangeText(curr_score);
	}
	if (action == BLUE_COUNTER)
	{
		char curr_score[50];
		sprintf_s(curr_score, "%d", App->entitymanager->blue_counter);

		ChangeText(curr_score);
	}
	if (action == GREEN_COUNTER)
	{
		char curr_score[50];
		sprintf_s(curr_score, "%d", App->entitymanager->green_counter);

		ChangeText(curr_score);
	}
	if (action == PLAYER_NAME)
	{
		position.x = App->render->camera.x + App->entitymanager->player_ref->position.x + App->entitymanager->player_ref->rect.w / 2 - rect[IDLE].w / 2;
		position.y = App->render->camera.y + App->entitymanager->player_ref->position.y - 20;

		return true;
	}

	return true;
}


void Label::Draw(SDL_Texture* sprites)
{
	sprites = text;
	App->render->Blit(text, position.x, position.y,NULL,0);
}

void Label::ChangeText(const char * string)
{
	if (text != nullptr)
	{
		App->tex->UnLoad(text);
	}

	text = App->font->Print(string, { 0,0,0,255 }, App->font->default);
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
}
