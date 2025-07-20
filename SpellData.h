#pragma once
#include <string>
#include "SpellIdentifiers.h"
#include "TextureIdentifiers.h"

struct HealingSpell
{
	float healValue = 0.f;
	int bonusHpRegen = 0;
	int bonusManaRegen = 0;
};

struct ProjectileSpell
{
	int minDmg = 0;
	int maxDmg = 1;
	int shotAmount = 1; //how many projectiles are spawned per shot
	int range = 1; //in tiles
	float speed = 0.f; //px per sec
	bool pierce = false; //'continues attacking after impact with first target'
	int maxTargets = 1; //if pierce == true
	SpellEffect effect = SpellEffect::None; //eventual effect on hit
};

struct SpellData
{
	SpellType type = SpellType::Heal;
	SpellIdentifier spellId = SpellIdentifier::QuickHeal;
	int cost = 0; //manacost or maybe in future hp cost?
	int cooldown = 0; //in milliseconds
	int castTime = 0; //in ms
	int duration = 0; //how long does the spelll last

	std::optional<HealingSpell> healing;
	std::optional<ProjectileSpell> projectile;
};

struct SpellInstance
{
	const SpellData* data = nullptr;
	int cooldownRemaining = 0;
	int durationRemaining = 0;
};

struct SpellInfo
{
	std::string name;
	std::string shortDescription;
	std::string longDescription;
	TextureIdentifier textureId;
	std::vector<std::pair<std::string, std::string>> attributes;
};

struct SpellDefinition
{
	SpellData* data = nullptr;
	SpellInfo spellInfo;
};