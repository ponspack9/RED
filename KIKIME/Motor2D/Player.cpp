#include "j1App.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "p2Log.h"
#include "Player.h"
#include "Brofiler/Brofiler.h"
#include "Player.h"

Player::Player(iPoint pos, Entity * e, SDL_Texture * sprites) : Entity(type)
{
	name.create("player");

	this->graphics = sprites;
	this->type = type;

	position = pos;
	rect = { pos.x,pos.y,e->rect.w,e->rect.h };
	speed = e->speed;

	level_finished = false;
	on_floor = false;
	is_jumping = false;
	is_falling = true;
	dead = false;

	can_move_right = true;
	can_move_left = true;
	can_move_up = true;
	can_move_down = true;

	collider = App->collision->AddCollider(rect, COLLIDER_PLAYER);
}

Player::~Player()
{

}

bool Player::PreUpdate()
{
	BROFILER_CATEGORY("Player->PreUpdate", Profiler::Color::BlueViolet)

	if (!godmode)Move();

	return true;
}

bool Player::Update(float dt)
{
	BROFILER_CATEGORY("Player->Update", Profiler::Color::BlueViolet)
	//vertical_movement	= true;
	//horizontal_movement = true;

	if (level_finished) App->NextLevel();
	
	else
	{
		if (!godmode)
		{
			//Move();
			if (dx > 0 && !can_move_right) {
				MovePlayer(-dx, 0);
				//horizontal_movement = false;
			}
			else if (dx < 0 && !can_move_left) {
				MovePlayer(-dx, 0);
				//horizontal_movement = false;
			}
			if (dy > 0 && !can_move_down) {
				MovePlayer(0, -dy);
				//vertical_movement = false;
			}
			else if (dy < 0 && !can_move_up) {
				MovePlayer(0, -dy);
				//vertical_movement = false;
			}
			
			horizontal_collided = false;
			vertical_collided = false;
			
			PlayerAnimations();
			on_floor = false;
		}
		else
		{
			current_animation = &god;
			MoveFree();
		}

	}

	Draw();
	return true;
}

bool Player::PostUpdate()
{
	BROFILER_CATEGORY("Player->PostUpdate", Profiler::Color::BlueViolet)
	//App->render->MoveCamera(-dx, -dy);
	if (position.x > App->map->world_limits.x || position.y > App->map->world_limits.y || position.x < 0 || position.y < 0)
		dead = true;

	if (dead)
	{
		App->SoftRestartLevel();
		dead = false;
	}
	return true;
}

void Player::Die()
{
	dead = true;
	current_animation = &death;
}

void Player::Draw()
{
	if (move_left)
	{
		App->render->Blit(graphics, position.x, position.y, &current_animation->GetCurrentFrame(), 1, 0, SDL_FLIP_HORIZONTAL);
	}
	else if (djump)
	{
		App->render->Blit(graphics, position.x, position.y, &current_animation->GetCurrentFrame(), 1, 0, SDL_FLIP_VERTICAL);
	}
	else {
		App->render->Blit(graphics, position.x, position.y, &current_animation->GetCurrentFrame());
	}

}

bool Player::MovePlayer(float vel_x, float vel_y)
{
	position.x += vel_x;
	rect.x = position.x;
	
	position.y += vel_y;
	rect.y = position.y;

	if (collider != nullptr) {
		collider			->SetPos(position.x, position.y);
		collider_ray_right	->SetPos(position.x + collider_offset, position.y + collider_offset);
		collider_ray_left	->SetPos(position.x - collider_offset, position.y + collider_offset);
		collider_ray_up		->SetPos(position.x + collider_offset, position.y - collider_offset);
		collider_ray_down	->SetPos(position.x + collider_offset, position.y + collider_offset);
	}
	else LOG("MISSING PLAYER COLLIDER");

	return true;
}


void Player::Move()
{
	/*if (vertical_collided && on_floor)	
		on_floor = last_collision == collider_ray_down->type;*/
	if (!vertical_collided && !can_move_down)	can_move_down = true;
	if (!vertical_collided && !can_move_up)		can_move_up = true;

	if (!horizontal_collided && !can_move_right)	can_move_right = true;
	if (!horizontal_collided && !can_move_left)		can_move_left = true;

	

	can_move_down &= !on_floor && !is_jumping && !djump;

	dx = 0;
	dy = 0;
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		Die();
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && can_move_right)
	{
		move_right = true;
		move_left = false;
		dx += speed.x;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
	{
		move_right = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && can_move_left)
	{
		move_right = false;
		move_left = true;
		dx -= speed.x;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
	{
		move_left = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		if(on_floor && !is_jumping)
		{
		Jump();
		//LOG("JUMP");
	}
	if ((App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)&& !on_floor && is_falling && !aux_djump && !djump)
	{
		DoubleJump();
	}
	if (is_jumping)
	{
		is_jumping = Jump();
		//last iteration jumping
		if (!is_jumping) {
			//is_falling = true;
		}
	}
	else if (djump)
	{
		djump = DoubleJump();
		if (!djump) {
			//is_falling = true;
		}
	}
	else if (can_move_down)
	{
		is_falling = true;

		falling_y += gravity/2;
		if (falling_y >= max_speed_y) falling_y = max_speed_y;

		dy = falling_y;
	}
	else if (!is_falling)
	{
		falling_y = 0;
	}

	
	MovePlayer(dx, dy);
		
}



void Player::OnCollision(Collider * c1, Collider * c2)
{
	*collider_identifier = c2;

	if (c1->type == COLLIDER_RAY_RIGHT && c2->type == COLLIDER_FLOOR) {
		can_move_right = false;
		horizontal_collided = true;
		//LOG("COLLIDED_RAY_RIGHT");
	}
	if (c1->type == COLLIDER_RAY_LEFT && c2->type == COLLIDER_FLOOR) {
		can_move_left = false;
		horizontal_collided = true;
		//LOG("COLLIDED_RAY_LEFT");

	}
	if (c1->type == COLLIDER_RAY_UP && c2->type == COLLIDER_FLOOR) {
		can_move_up = false;
		can_move_down = true;
		on_floor = false;
		vertical_collided = true;
		//LOG("COLLIDED_RAY_UP");

	}
	if (c1->type == COLLIDER_RAY_DOWN && c2->type == COLLIDER_FLOOR) {
		can_move_down = false;
		on_floor = true;
		is_falling = false;
		djump = false;
		aux_djump = false;
		vertical_collided = true;
		//LOG("COLLIDED_RAY_DOWN");

	}
	if (!can_move_down && !can_move_left && !can_move_right) {
		can_move_left = true;
		can_move_right = true;
		is_falling = false;
		on_floor = true;
		//LOG("SPECIAL CASE");
	}

	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_END && !level_finished) {
		level_finished = true;
	}
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_DEATH) {
		Die();
	}

}
bool Player::Jump()
{
	if (!is_jumping)
	{
		is_jumping = true;
		is_falling = false;
		on_floor = false;

		jumpspeed = 15;
		return true;
	}
	else
	{
		dy -= jumpspeed;
		jumpspeed -= gravity / 2;
	}
	return (jumpspeed >= 0);
}

bool Player::DoubleJump()
{
	if (!djump)
	{
		djump = true;
		aux_djump = true;
		is_falling = false;
		is_jumping = false;

		jumpspeed = 20;
		return true;
	}
	else
	{
		dx = 0;
		dy -= jumpspeed;
		jumpspeed -= gravity / 2;
	}
	return (jumpspeed >= 0);
}

void Player::MoveFree()
{
	dx = 0;
	dy = 0;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		dx += speed.x + 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		dx -= speed.x + 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		dy += speed.y + 5;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		dy -= speed.y + 5;
	}

	position.x += dx;
	position.y += dy;
  
	vertical_collided = false;
}

void Player::PlayerAnimations()
{
	if (is_falling && !is_jumping && !djump && !on_floor)
	{
		current_animation = &fall;
	}
	else if (!is_jumping && !move_left && !move_right && !djump && !is_falling)
	{
		current_animation = &idle;
	}
	if (!is_jumping && move_right && !is_falling)
	{
		current_animation = &walk;
	}
	else if (!is_jumping && move_left && !is_falling)
	{
		current_animation = &walk;
	}
	if (is_jumping && !is_falling && !djump)
	{
		current_animation = &jump;
	}
	else if (djump && !is_falling && !is_jumping)
	{
		current_animation = &doublejump;
	}
	if (dead)
	{
		current_animation = &death;
	}

	rect.w = current_animation->GetCurrentFrame().w;
	rect.h = current_animation->GetCurrentFrame().h;
}