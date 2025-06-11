#pragma once
#include "ISystem.h"
class TileMap;

class AttackSystem : public ISystem
{
public:
	AttackSystem(SystemContext& systemContext, const TileMap& tileMap);
	virtual void update(const sf::Time& deltaTime) override;
private:
	void registerToEvents();
	void registerToStartAttackingEvent();
	void registerToAttackFinishedEvent();
	void startAttack(Entity& entity);
	void removeFinishedEntities();

	std::vector<Entity*> getHitEntities(const Entity& entity, const AttackData& data) const;
private:
	const TileMap& mTileMap;
	std::vector<Entity*> mFinishedEntities;
};

