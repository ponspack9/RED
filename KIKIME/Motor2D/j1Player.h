#pragma once
#include "SDL/include/SDL.h"
#include "j1Module.h"
#include "j1Collision.h"
#include "Animation.h"

class j1Player : public j1Module
{

public:

	j1Player();
	~j1Player();

	bool Awake(pugi::xml_node& config);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void Die();
	void Draw();
	bool MovePlayer(float vel_x, float vel_y);
	void Move();
	void MoveFree();
	bool Jump();
	bool DoubleJump();
	void PlayerAnimations();

	bool Save(pugi::xml_node& node);
	bool Load(pugi::xml_node& node);

	void OnCollision(Collider* c1, Collider* c2);

	void OnCollisionLine(Collider* c, int x1, int y1, int x2, int y2);

public:
	p2SString texture_path;
	SDL_Texture* graphics = nullptr;
	Animation* current_animation = nullptr;
	Animation idle;
	Animation walk;		
	Animation jump;	
	Animation doublejump;
	Animation fall;
	Animation death;
	Animation god;
	float def_anim_speed;
	SDL_Rect anim_rect;

	bool level_finished;
	bool on_floor;
	bool dead;

	//Animations
	bool move_left;
	bool move_right;
	bool is_jumping;
	bool is_falling;
	bool djump;
	bool aux_djump;
	float dx = 0;
	float dy = 0;
	float falling_y;
	int max_speed_y;
	float gravity;
	float jumpspeed;
	bool top, bottom, left, right;
	bool can_left, can_right;
	fPoint position;
	iPoint init_pos1;
	iPoint init_pos2;
	fPoint speed;
	SDL_Rect player_rect;
	bool godmode = false;
	Collider* player_collider;

	//NEW approach
	bool can_move_right;
	bool can_move_left;
	bool can_move_up;
	bool can_move_down;

	bool vertical_collided;
	bool horizontal_collided;

	Collider* collider_ray_right;
	Collider* collider_ray_left;
	Collider* collider_ray_up;
	Collider* collider_ray_down;

	Collider* collider_identifier;

	int collider_offset = 0;
	COLLIDER_TYPE last_collision;

	int r, g, b;
};