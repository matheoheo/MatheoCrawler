#pragma once
#include <string>
#include "SpellIdentifiers.h"
struct ProjectileSpell
{
	int minDmg = 0;
	int maxDmg = 1;
	int shotAmount = 1; //how many projectiles are spawned per shot
	int range = 1; //in tiles
	float speed = 0.f; //px per sec
	bool pierce = false; //'continues attacking after impact with first target'
	int maxTargets = 1; //if pierce == true
};

struct SpellData
{
	SpellType type = SpellType::Heal;
	std::string name = "spell_default";
	int cost = 0; //manacost or maybe in future hp cost?
	int cooldown = 0; //in milliseconds
	int castTime = 0; //in ms
	
	float healValue = 0.f; //in percents
	int duration = 0; //how long does the spelll last
	int bonusHpRegen = 0;
	int bonusManaRegen = 0;

	std::optional<ProjectileSpell> projectile;
};

struct SpellInstance
{
	const SpellData* data = nullptr;
	int cooldownRemaining = 0;
	int durationRemaining = 0;
};