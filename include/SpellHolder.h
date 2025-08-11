#pragma once
#include "SpellIdentifiers.h"
#include "SpellData.h"


//SpellHolder, at first version wasn't a Singleton Instance.
//But, when creating Shop system, i decided it is the best way to retrieve spell definitions.
//Because, if i wanted to have SpellData in SpellcraftShopCategory, then i would have to create a parametr chain:
//from UIComponent -> Shop -> IShopCategory -> SpellcraftShopCategory; and only at the end it would be used.
//Based on this, i intended to make this data class as Singleton.
class SpellHolder
{
public:
	static SpellHolder& getInstance();
	SpellData& get(SpellIdentifier id);
	const SpellData& get(SpellIdentifier id) const;
	const SpellDefinition& getDefinition(SpellIdentifier id) const;
private:
	SpellHolder();

	void initalizeSpells();
	void createQuickHeal();
	void createMajorHeal();
	void createHealthRegen();
	void createManaRegen();

	void createWaterBall();
	void createPureProjectile();
	void createFireball();
	void createBloodball();

	//aoe spells
	void createFrostPillar();
	void createBladeDance();
	void createToxicCloud();
	void createThunderstorm();

	void createMorannaProjectile();
private:
	std::unordered_map<SpellIdentifier, SpellData> mSpellsMap;
	std::unordered_map<SpellIdentifier, SpellDefinition> mDefinitions;
};

