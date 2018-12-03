#include "Label.h"

Label::Label(iPoint pos, UIType type, p2SString string) : UIElement(type)
{
	this->initial_pos = pos;
	this->position = pos;
	this->type = type;
	this->state = IDLE;

	this->rect[IDLE].x = position.x;
	this->rect[IDLE].y = position.y;

	this->string = string;

	text = App->font->Print(string.GetString(), { 255,0,0,255 }, App->font->default);
	App->font->CalcSize(string.GetString(), rect[IDLE].w, rect[IDLE].h, App->font->default);
}

bool Label::PreUpdate()
{
	position.x = initial_pos.x - App->render->camera.x - rect[IDLE].w / 2;
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
