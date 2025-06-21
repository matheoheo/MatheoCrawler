#pragma once
#include "SpellIdentifiers.h"
#include "SpellData.h"

class SpellHolder
{
public:
	SpellHolder();
	void initalizeSpells();
	const SpellData& get(SpellIdentifier id) const;
	const SpellDefinition& getDefinition(SpellIdentifier id) const;
private:
	void createQuickHeal();
	void createMajorHeal();
	void createHealthRegen();
	void createManaRegen();

	void createWaterBall();
	void createPureProjectile();
	void createFireball();
	void createBloodball();
private:
	std::unordered_map<SpellIdentifier, SpellData> mSpellsMap;
	std::unordered_map<SpellIdentifier, SpellDefinition> mDefinitions;
};

