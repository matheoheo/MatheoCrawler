#include "pch.h"
#include "EntityDataRepository.h"

EntityDataRepository::EntityDataRepository()
{
	createEntityData();
}

const EntityDataRepository::BaseEntityData& EntityDataRepository::get(EntityType entType) const
{
	assert(mEntityData.contains(entType));

	return mEntityData.at(entType);
}

void EntityDataRepository::createEntityData()
{
    createPlayerEntityData();
    createSkletorusEntityData();
    createBonvikEntityData();
}

void EntityDataRepository::createPlayerEntityData()
{
	BaseEntityData data;
	data.tag = "Player_Default";
	data.moveSpeed = 165.f;
	data.fovRange = 5;

	data.combatStats.cAttackDamage = 19;
	data.combatStats.cAttackRange = 1;
	data.combatStats.cAttackSpeed = 1.3f;
	data.combatStats.cDefence = 8;
	data.combatStats.cHealth = 188;
	data.combatStats.cMaxHealth = data.combatStats.cHealth;
	data.combatStats.cMana = 100;
	data.combatStats.cMaxMana = data.combatStats.cMana;
	data.cAttackDataMap = std::move(createPlayerAttackDataMap());

	mEntityData.emplace(EntityType::Player, std::move(data));
}

void EntityDataRepository::createSkletorusEntityData()
{
	BaseEntityData data;
	data.tag = "Skletorus";
	data.moveSpeed = 115.f;
	data.fovRange = 3;

	data.combatStats.cAttackDamage = 6;
	data.combatStats.cAttackRange = 1;
	data.combatStats.cAttackSpeed = 1.1f;
	data.combatStats.cDefence = 2;
	data.combatStats.cHealth = 43;
	data.combatStats.cMaxHealth = data.combatStats.cHealth;

	data.cAttackDataMap = createSimpleMeleeAttackDataMap();
	mEntityData.emplace(EntityType::Skletorus, std::move(data));
}

void EntityDataRepository::createBonvikEntityData()
{
	BaseEntityData data;
	data.tag = "Bonvik";
	data.moveSpeed = 125.f;
	data.fovRange = 4;

	data.combatStats.cAttackDamage = 11;
	data.combatStats.cAttackRange = 1;
	data.combatStats.cAttackSpeed = 0.9f;
	data.combatStats.cDefence = 4;
	data.combatStats.cHealth = 51;
	data.combatStats.cMaxHealth = data.combatStats.cHealth;

	data.cAttackDataMap = createDualStrikeMeleeAttackDataMap();
	mEntityData.emplace(EntityType::Bonvik, std::move(data));
}

EntityDataRepository::AttackDataMap EntityDataRepository::createSimpleMeleeAttackDataMap() const
{
	AttackDataMap result;
	AttackData att;
	att.damageMultiplier = 1.0f;
	att.speedMultiplier = 1.0f;

	att.hitOffsets[Direction::Up] = std::vector<sf::Vector2i>{ { 0, -1} };
	att.hitOffsets[Direction::Left] = std::vector<sf::Vector2i>{ {-1,  0} };
	att.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{ { 0,  1} };
	att.hitOffsets[Direction::Right] = std::vector<sf::Vector2i>{ { 1,  0} };

	result[AnimationIdentifier::Attack1] = std::move(att);
	return result;
}

EntityDataRepository::AttackDataMap EntityDataRepository::createDualStrikeMeleeAttackDataMap() const
{
	auto result = createSimpleMeleeAttackDataMap();
	AttackData att2;
	att2.damageMultiplier = 1.1f;
	att2.speedMultiplier = 0.8f;
	att2.hitOffsets = result[AnimationIdentifier::Attack1].hitOffsets;

	result[AnimationIdentifier::Attack2] = std::move(att2);
	return result;
}

EntityDataRepository::AttackDataMap EntityDataRepository::createPlayerAttackDataMap() const
{
	AttackDataMap result;
	AttackData att1, att2, att3;
	att1.damageMultiplier = 1.0f;
	att1.speedMultiplier = 1.0f;
	att1.hitOffsets[Direction::Up] = std::vector<sf::Vector2i>{ {-1, 0},  { 0, -1} };
	att1.hitOffsets[Direction::Left] = std::vector<sf::Vector2i>{ {-1, 0},  { 0, -1} };
	att1.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{ { 1, 0},  { 0,  1} };
	att1.hitOffsets[Direction::Right] = std::vector<sf::Vector2i>{ { 1, 0},  { 0,  1} };

	att2.damageMultiplier = 1.0f;
	att2.speedMultiplier = 1.0f;
	att2.hitOffsets[Direction::Up] = std::vector<sf::Vector2i>{ { 1, 0}, { 0, -1} };
	att2.hitOffsets[Direction::Left] = std::vector<sf::Vector2i>{ {-1, 0}, { 0,  1} };
	att2.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{ {-1, 0}, { 0,  1} };
	att2.hitOffsets[Direction::Right] = std::vector<sf::Vector2i>{ { 1, 0}, { 0, -1} };

	att3.damageMultiplier = 1.35f;
	att3.speedMultiplier = 1.1f;
	att3.hitOffsets[Direction::Up] = std::vector<sf::Vector2i>{ { 0, -1} };
	att3.hitOffsets[Direction::Left] = std::vector<sf::Vector2i>{ {-1,  0} };
	att3.hitOffsets[Direction::Bottom] = std::vector<sf::Vector2i>{ { 0,  1} };
	att3.hitOffsets[Direction::Right] = std::vector<sf::Vector2i>{ { 1,  0} };

	result[AnimationIdentifier::Attack1] = std::move(att1);
	result[AnimationIdentifier::Attack2] = std::move(att2);
	result[AnimationIdentifier::Attack3] = std::move(att3);

	return result;
}
