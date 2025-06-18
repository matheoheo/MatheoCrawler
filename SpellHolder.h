#pragma once
#include "SpellIdentifiers.h"
#include "SpellData.h"

class SpellHolder
{
public:
	SpellHolder();
	void initalizeSpells();

	const SpellData& get(SpellIdentifier id) const;
private:
	void createBasicHeal();
private:
	std::unordered_map<SpellIdentifier, SpellData> mSpellsMap;
};

