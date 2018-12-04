#pragma once
#include "Entity.h"

enum coinType {
	GREEN_DIAMOND,
	BLUE_DIAMOND,
	HEART,

	NOTYPE
};

class Coin : public Entity
{
public:
	Coin() {
		coin_type = coinType::NOTYPE;
		points = -123;
	}

	Coin(iPoint pos, Coin* e) : Entity(pos, e) {

		coin_type	= e->coin_type;
		points		= e->points;

		switch (coin_type)
		{
		case GREEN_DIAMOND:
			LOG("GREEN_DIAMOND CREATED");
			break;
		case BLUE_DIAMOND:
			LOG("BLUE_DIAMOND CREATED");
			break;
		case HEART:
			LOG("HEART CREATED");
			break;
		default:
			break;
		}
	}


	~Coin() {}

public:

	coinType coin_type;
	int points;
};

