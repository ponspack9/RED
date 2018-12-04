#pragma once
#include "Entity.h"

enum coinType {
	GREEN_DIAMOND,
	BLUE_DIAMOND,

	NOTYPE
};

class Coin : public Entity
{
public:
	Coin() {
		coin_type = coinType::NOTYPE;
		points = -123;
	}
	Coin(iPoint pos, Entity* e) : Entity(pos, e) {}


	~Coin() {}

public:

	coinType coin_type;
	int points;
};

