#pragma once
#include <SFML/Graphics.hpp>
#include "Directions.h"


struct BeamData
{
	int damage = 0;
	int length = 0; //in tiles
	int chargeTime = 1000;
	sf::Color outerColor;
	sf::Color innerColor;
	sf::Vector2f casterPos;
	Direction dir;
};