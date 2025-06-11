#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include "Directions.h"

struct AttackData
{
	using HitOffsetsVec = std::vector<sf::Vector2i>;
	float damageMultiplier = 1.f;
	float speedMultiplier = 1.f;
	bool friendlyFire = false;
	std::unordered_map<Direction, HitOffsetsVec> hitOffsets;
};