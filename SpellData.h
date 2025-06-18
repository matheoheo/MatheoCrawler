#pragma once
#include <string>
#include "SpellIdentifiers.h"

struct SpellData
{
	SpellType type = SpellType::Heal;
	std::string name = "spell_default";
	int cost = 0; //manacost or maybe in future hp cost?
	int cooldown = 0; //in milliseconds
	int castTime = 0; //in ms
	float healValue = 0.f; //in percents

	int minDmg = 0; //in case of attacking
	int maxDmg = 0; 
};

struct SpellInstance
{
	const SpellData* data = nullptr;
	int cooldownRemaining = 0;
};