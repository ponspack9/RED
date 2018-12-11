#include "Image.h"

Image::Image(ActionType action,iPoint pos, SDL_Rect rect, Image img,UIType type, UIElement* parent, bool visible) : UIElement(type, parent, visible)

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

	this->idle = img.idle;
	this->blue_shine = img.blue_shine;
	this->green_shine = img.green_shine;
}

void Image::Draw(SDL_Texture * sprites)
{
	int phi;
	int temp;
	switch (this->action)
	{
	case INFO:

		App->render->Blit(sprites, position.x, position.y, &rect[state],0);
		break;

	case LIFE_SYSTEM:

		phi = 40;
		temp = position.x;

		for (int i = 0; i < App->entitymanager->player_ref->lifes; i++)
		{
			App->render->Blit(sprites, position.x, position.y, &rect[state],0);
			position.x += phi;
		}
		position.x = temp;
		break;
	case LAST_DEATH:

		App->render->Blit(sprites, position.x, position.y, &rect[state], 1);
		break;
	case DYNAMIC_INFO:

		if (&current_animation->GetCurrentFrame() == NULL)
		{
			current_animation = &idle;
			App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 0);
		}
		else
		{
			App->render->Blit(sprites, position.x, position.y, &current_animation->GetCurrentFrame(), 0);
		}
		break;
	default:
		App->render->Blit(sprites, position.x, position.y, &rect[state], 0);
		break;
	}
}

