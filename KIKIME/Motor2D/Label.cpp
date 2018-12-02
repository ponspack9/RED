#include "Label.h"

Label::Label(iPoint pos, UIType type, p2SString string) : UIElement(type)
{
	this->position = pos;
	this->type = type;

	this->string = string;
}

bool Label::PreUpdate()
{
	int w, h;

	text = App->font->Print(string.GetString(), { 255,0,0,255 }, App->font->default);
	App->font->CalcSize(string.GetString(), w, h, App->font->default);

	position.x = App->render->viewport.w / 2 - App->render->camera.x - w / 2;
	position.y = App->render->viewport.h / 13 - App->render->camera.y;

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
