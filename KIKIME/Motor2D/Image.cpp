#include "Image.h"

Image::Image(iPoint pos, SDL_Rect rect, UIType type, ActionType action) : UIElement(type)
{
	this->initial_pos = pos;
	this->rect[IDLE] = rect;
	this->rect[HOVER] = rect;
	this->rect[CLICK_DOWN] = rect;
	this->rect[CLICK_UP] = rect;

	this->position = pos;
	this->type = type;
	
	this->state = IDLE;
	this->action = action;
}

bool Image::PreUpdate()
{
	position.x = initial_pos.x - App->render->camera.x;
	position.y = initial_pos.y - App->render->camera.y;

	return true;
}

bool Image::PostUpdate()
{
	return true;
}

void Image::Draw(SDL_Texture * sprites)
{
	sprites = App->entitymanager->otherTex;

	switch (this->action)
	{
	case INFO:

		App->render->Blit(sprites, position.x, position.y, &rect[state]);
		break;

	case LIFE_SYSTEM:

		int phi = 40;
		int temp = position.x;

		for (int i = 0; i < App->entitymanager->player_ref->lifes; i++)
		{
			App->render->Blit(sprites, position.x, position.y, &rect[state]);
			position.x += phi;
		}
		position.x = temp;
		break;
	}
}
