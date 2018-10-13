#include "j1Collision.h"
#include "j1App.h"
#include "j1Input.h"
#include "p2Point.h"
#include "j1Player.h"
#include "j1Render.h"
#include "p2Log.h"


j1Player::j1Player()
{
	name.create("player");
}


j1Player::~j1Player()
{

}

bool j1Player::Awake(pugi::xml_node & config)
{
	pugi::xml_node player_node = config;

	//LVL 1 INITIAL POSITION
	init_pos1.x = player_node.child("lvl1").attribute("x").as_float();
	init_pos1.y = player_node.child("lvl1").attribute("y").as_float();

	//LVL 2 INITIAL POSITION
	init_pos2.x = player_node.child("lvl2").attribute("x").as_float();
	init_pos2.y = player_node.child("lvl2").attribute("y").as_float();

	////PLAYER RECT DIMENSIONS
	player_rect.w = player_node.child("rect").attribute("width").as_uint();
	player_rect.h = player_node.child("rect").attribute("height").as_uint();
	
	//PL. COLLIDER
	player_collider = App->collision->AddCollider(player_rect, COLLIDER_PLAYER, this);
	//SDL_Rect shade = { player_collider->rect.x,player_collider->rect.y,
						//player_collider->rect.w, 5 };
	//shade_collider  = App->collision->AddCollider(shade,COLLIDER_PLAYER, this);

	//SCROLL AND JUMPSPEED (CONST)
	speed.x = player_node.child("speed").attribute("scrollspeed").as_float();
	speed.y = player_node.child("speed").attribute("jumpspeed").as_float();

	gravity = player_node.child("gravity").attribute("value").as_float();
	
	//LOG("%d  %d", player_rect.h, player_rect.w);
	LOG("%d  %d", speed.x, speed.y);

	return true;
}

bool j1Player::Start()
{
	//PLACING PLAYER AT INITIAL POS
	position.x = init_pos1.x;
	position.y = init_pos1.y;

	//PLACING PLAYER RECT
	player_rect.x = position.x;
	player_rect.y = position.y;

	max_speed_y = speed.y;
	on_floor = false;
	is_jumping = false;
	r = 255;
	g = 0;
	b = 0;

	return true;
}

bool j1Player::Update(float dt)
{
	Move();
		
	player_collider->SetPos(position.x, position.y);
	Draw();
	return true;
}

void j1Player::Draw()
{
	App->render->DrawQuad(player_rect, r,g,b);
	g = 0;

}

////////////////////////////////////////////////////////////////////////////////////////////
void j1Player::Move()
{
	if (!have_collided && on_floor) on_floor = false;
	float dx = 0;
	float dy = 0;
	
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) 
	{
		dx += speed.x;
	}

	
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		dx -= speed.x;
	}

	/*if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		if (!on_floor)
		{
			position.y += (speed.y + 2);
			speed.y += gravity;
		}
	}*/


	if (is_jumping)
	{
		is_jumping = Jump();
		LOG("JUMPING");
	}
	else if (!on_floor)
	{
		dy += gravity;
		//on_floor = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && on_floor)
	{
		Jump();
		LOG("JUMP");

	}

	position.x += dx;
	position.y += dy;

	player_rect.x = position.x;
	player_rect.y = position.y;

	player_collider->SetPos(position.x, position.y);
	//shade_collider->SetPos(position.x, position.y+ player_collider->rect.h);

	have_collided = false;


//	/if (is_jumping && !move_left && !move_right)
//	/{
//	/	position.y -= aux_speed_y;
//	/	aux_speed_y -= 0.2;
//	/	if (aux_speed_y == 0)
//	/	{
//	/		on_top = true;
//	/		position.y += aux_speed_y;
//	/		aux_speed_y += 0.2;
//	/		if (aux_speed_y == speed.y)
//	/		{
//	/			position.y += aux_speed_y;
//	/		}
//  /
//	/	}
//  /
//	/}
//	/if (is_jumping && move_left && !move_right)
//	/{
//	/	position.x -= speed.x;
//	/	position.y -= aux_speed_y;
//	/	aux_speed_y -= 0.2;
//	/	if (aux_speed_y == 0)
//	/	{
//	/		on_top = true;
//	/		position.y += aux_speed_y;
//	/		aux_speed_y += 0.1;
//	/		if (aux_speed_y == speed.y)
//	/		{
//	/			position.y += aux_speed_y;
//	/		}
//  /
//	/	}
//	/}
//	/if (is_jumping && !move_left && move_right)
//	/{
//	/	position.x -= speed.x;
//	/	position.y -= aux_speed_y;
//	/	aux_speed_y -= 0.1;
//	/	if (aux_speed_y == 0)
//	/	{
//	/		on_top = true;
//	/		position.y += aux_speed_y;
//	/		aux_speed_y += 0.1;
//	/		if (aux_speed_y == speed.y)
//	/		{
//	/			position.y += aux_speed_y;
//	/		}
//  /
//	/	}
//	/}
//	/if (is_jumping && move_left && move_right)
//	/{
//	/	position.y -= aux_speed_y;
//	/	aux_speed_y -= 0.1;
//	/	if (aux_speed_y == 0)
//	/	{
//	/		on_top = true;
//	/		position.y += aux_speed_y;
//	/		aux_speed_y += 0.1;
//	/	}
//	/	if (aux_speed_y == speed.y)
//	/	{
//	/		position.y += aux_speed_y;
//	/	}
//  /
//  /
//	///if (!is_jumping && !on_floor)
//	///{
//	///	on_top = true;
//	///	position.y += aux_speed_y;
//	///	aux_speed_y += 0.1;
//	///	if (aux_speed_y == speed.y)
//	///	{
//	///		position.y += aux_speed_y;
//	///	}
//	///}
//  //
}

bool j1Player::Jump()
{
	if (!is_jumping)
	{
		is_jumping = true;
		jumpspeed = 5;
		on_floor = false;
		return true;
	}
	else {
		position.y -= jumpspeed;
		jumpspeed -= 0.2;

	}
	return (jumpspeed>=0);
}

bool j1Player::Load(pugi::xml_node & node)
{
	LOG("Loading PLAYER");

	position.x = node.child("player").child("position").attribute("x").as_int();
	position.y = node.child("player").child("position").attribute("y").as_int();

	return true;
}


bool j1Player::Save(pugi::xml_node & node)
{
	LOG("Saving PLAYER");

	pugi::xml_node pl_node = node.append_child("position");

	pl_node.append_attribute("x") = position.x;
	pl_node.append_attribute("y") = position.y;

	//pl_node.append_attribute("x") = init_pos1.x;
	//pl_node.append_attribute("y") = init_pos1.y;

	LOG("playerX: %d - %d \n playerY: %d - %d", init_pos1.x, pl_node.attribute("x").as_int(), init_pos1.y, pl_node.attribute("y").as_int());

	return true;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	have_collided = true;
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_FLOOR && !on_floor) {

		on_floor = true;
		//last_collision = COLLIDER_FLOOR;
	}
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_WALL) {

		can_keep_moving = false;
	}


}

void j1Player::OnCollisionLine(Collider * c, int x1, int y1, int x2, int y2)
{
	//LOG("COLLISION LINE");
	g = 255;
}



