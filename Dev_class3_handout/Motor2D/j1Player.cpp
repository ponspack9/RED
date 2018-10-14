#include "j1App.h"
#include "j1Collision.h"
#include "j1Input.h"
#include "p2Point.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "p2Log.h"
#include "j1Player.h"


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

	////PLAYER RECT DIMENSIONS
	player_rect.w = player_node.child("rect").attribute("width").as_uint();
	player_rect.h = player_node.child("rect").attribute("height").as_uint();


	//SCROLL AND JUMPSPEED (CONST)
	speed.x = player_node.child("speed").attribute("scrollspeed").as_float();
	speed.y = player_node.child("speed").attribute("jumpspeed").as_float();
	def_anim_speed = player_node.child("speed").attribute("defaultAnimationSpeed").as_float();

	gravity = player_node.child("gravity").attribute("value").as_float();

	// Parsing animations ----------------
	pugi::xml_node textureAtlas = player_node.child("TextureAtlas");
	texture_path = (textureAtlas.attribute("imagePath").as_string());

	SDL_Rect r;
	float node_speed = -1;

	// IDLE
	pugi::xml_node n = textureAtlas.child("idle");
	for (n; n; n = n.next_sibling("idle"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		idle.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	idle.speed = (node_speed <= 0) ? def_anim_speed : node_speed;

	// WALK
	n = textureAtlas.child("walk");
	for (n; n; n = n.next_sibling("walk"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		walk.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	walk.speed = (node_speed <= 0) ? def_anim_speed : node_speed;

	// JUMP
	n = textureAtlas.child("jump");
	for (n; n; n = n.next_sibling("jump"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		jump.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	jump.speed = (node_speed <= 0) ? def_anim_speed : node_speed;

	//DEAD
	n = textureAtlas.child("death");
	for (n; n; n = n.next_sibling("death"))
	{
		r.x = n.attribute("x").as_int();
		r.y = n.attribute("y").as_int();
		r.w = n.attribute("width").as_int();
		r.h = n.attribute("height").as_int();
		death.PushBack(r);
	}
	node_speed = n.attribute("speed").as_float();
	death.speed = (node_speed <= 0) ? def_anim_speed : node_speed;

	// End parsing animations -----------------

	//LOG("%d  %d", player_rect.h, player_rect.w);
	LOG("%d  %d", speed.x, speed.y);
	//PL. COLLIDER
	player_collider = App->collision->AddCollider(player_rect, COLLIDER_PLAYER, this);
	wall = App->collision->AddCollider(player_rect, COLLIDER_NONE);
	return true;
}

bool j1Player::Start()
{
	graphics = App->tex->Load(texture_path.GetString());

	//PLACING PLAYER AT INITIAL POS
	position.x = App->map->start_collider->rect.x;
	position.y = App->map->start_collider->rect.y;

	current_animation = &death;

	max_speed_y = speed.y;
	level_finished = false;
	on_floor = false;
	is_jumping = false;

	r = 255;
	g = 0;
	b = 0;

	return true;
}

bool j1Player::Update(float dt)
{
	if (level_finished) App->NextLevel();
	else {
		if (!godmode)
		{
			Move();
			PlayerAnimations();
		}
		else
		{
			MoveFree();
		}

		if (player_collider != nullptr) {
			//player_collider->rect.w = current_animation->GetCurrentFrame().w;
			//player_collider->rect.h = current_animation->GetCurrentFrame().h;
			player_collider->SetPos(position.x, position.y);
		}
	}
	Draw();
	return true;
}

bool j1Player::PostUpdate()
{
	//App->render->MoveCamera(-dx, -dy);
	if (dead)
	{
		App->RestartLevel();

		dead = false;
	}
	return true;
}

bool j1Player::CleanUp()
{
	App->tex->UnLoad(graphics);
	if (player_collider) player_collider->to_delete = true;
	return true;
}

void j1Player::Draw()
{
	if (move_left)
	{
		App->render->Blit(graphics, position.x, position.y, &current_animation->GetCurrentFrame(), 1, 0, SDL_FLIP_HORIZONTAL);
	}
	else {
		App->render->Blit(graphics, position.x, position.y, &current_animation->GetCurrentFrame());
	}
	g = 0;

}


void j1Player::Move()
{
	float oldX = position.x;
	float oldY = position.y;
	bool collidedX = false;
	bool collidedY = false;

	if (!have_collided && on_floor) on_floor = false;
	if (!have_collided && left) left = false;
	if (!have_collided && right) right = false;
	if (!have_collided && top) top = false;
	if (!have_collided && bottom) bottom = false;

	if (have_collided && on_floor) {
		on_floor = wall->type == COLLIDER_FLOOR && !on_wall;
	}

	if (!have_collided && on_wall) on_wall = false;
	//if (!have_collided && on_wall) on_wall = false;
	dx = 0;
	dy = 0;
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		dead = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT && !right)
	{
		move_right = true;
		move_left = false;
		dx += speed.x;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
	{
		move_right = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT && !left)
	{
		move_right = false;
		move_left = true;
		dx -= speed.x;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
	{
		move_left = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN && on_floor)
	{
		Jump();
		LOG("JUMP");
	}

	if (is_jumping)
	{
		is_jumping = Jump();
	}
	else if (!on_floor && !is_jumping)
	{
		dy += gravity;
	}

	if (have_collided) {

		
		//Moving left
		if (dx < 0) {
			left = (position.x <= wall->rect.x + wall->rect.w) && wall->type != COLLIDER_FLOOR;
			LOG("LEEEEEEEEEEEEEEEEEEEEEEEFT");



			// Colliding also with the floor collider 
			/*bottom = (left && dy <= 0);
			can_left = left && !bottom || */

		}
		//Moving right
		else if (dx > 0) {
			right = (position.x + player_collider->rect.w >= wall->rect.x) && wall->type != COLLIDER_FLOOR;
			LOG("RIIIIIIIIIIIGHT");
		}


		//Moving up
		if (dy < 0) {
			top = (position.y <= wall->rect.y + wall->rect.h) && wall->type != COLLIDER_FLOOR;
		}
		//Moving down
		else if (dy > 0) {
			bottom = (position.y + player_collider->rect.h >= wall->rect.y) && wall->type != COLLIDER_FLOOR;
		}
	}

	//Actually moving
	position.x += dx;
	position.y += dy;

	//MovePlayer(-dx, -dy);

	player_rect.x = position.x;
	player_rect.y = position.y;

	have_collided = false;
}

bool j1Player::Jump()
{
	if (!is_jumping)
	{
		is_jumping = true;
		jumpspeed = 20;
		on_floor = false;
		return true;
	}
	else {
		dy -= jumpspeed;
		jumpspeed -= gravity;

	}
	return (jumpspeed >= 0);
}




void j1Player::MoveFree()
{
	current_animation = &idle;
	dx = 0;
	dy = 0;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		dx += speed.x;
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		dx -= speed.x;

	}

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		dy += speed.y;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		dy -= speed.y;
	}

	position.x += dx;
	position.y += dy;

	have_collided = false;

}

void j1Player::PlayerAnimations()
{
	if (!is_jumping && move_right)
	{
		current_animation = &walk;
	}
	if (!is_jumping && move_left)
	{
		current_animation = &walk;
	}
	if (!is_jumping && !move_left && !move_right)
	{
		current_animation = &idle;
	}
	if (dead)
	{
		current_animation = &death;
	}

	/*player_rect.x = position.x;
	player_rect.y = position.y;*/
	player_rect.w = current_animation->GetCurrentFrame().w;
	player_rect.h = current_animation->GetCurrentFrame().h;
}

void j1Player::OnCollision(Collider * c1, Collider * c2)
{
	have_collided = true;
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_FLOOR && !on_floor) {

		on_floor = true;
		*wall = (c2);
		LOG("TYPE: %d", wall->type);
		//last_collision = COLLIDER_FLOOR;
	}
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_WALL) {

		on_wall = true;
		*wall = (c2);
		LOG("TYPE: %d", wall->type);

	}
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_END && !level_finished) {
		level_finished = true;
	}

}

void j1Player::OnCollisionLine(Collider * c, int x1, int y1, int x2, int y2)
{
	LOG("COLLISION LINE");
	g = 255;
}

bool j1Player::Load(pugi::xml_node & node)
{
	LOG("Loading PLAYER");

	position.x = node.child("position").attribute("x").as_float();
	position.y = node.child("position").attribute("y").as_float();

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

	LOG("playerX: %f - %f \n playerY: %f - %f", position.x, pl_node.attribute("x").as_float(), position.y, pl_node.attribute("y").as_float());

	return true;
}