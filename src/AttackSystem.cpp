#include "pch.h"
#include "AttackSystem.h"
#include "Utilities.h"
#include "TileMap.h"
#include "Entity.h"

AttackSystem::AttackSystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void AttackSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		if (!Utilities::isEntityIdling(*entity))
			continue;
		startAttack(*entity);
	}
	removeFinishedEntities();

}

void AttackSystem::registerToEvents()
{
	registerToStartAttackingEvent();
	registerToAttackFinishedEvent();
}

void AttackSystem::registerToStartAttackingEvent()
{
	mSystemContext.eventManager.registerEvent<StartAttackingEvent>([this](const StartAttackingEvent& data)
		{
			if (isEntityAlreadyTracked(data.entity))
				return;
			if (!Utilities::isEntityIdling(data.entity))
				return;

			auto& entity = data.entity;
		
			data.entity.getComponent<AttackComponent>().cNextAttackId = data.animId;
			mTrackedEntities.push_back(&data.entity);
		});
}

void AttackSystem::registerToAttackFinishedEvent()
{
	mSystemContext.eventManager.registerEvent<AttackAnimationFinishedEvent>([this](const AttackAnimationFinishedEvent& data)
		{
			//ranged entities act differently - they spawn projectiles
			if (data.entity.hasComponent<RangedEnemyComponent>())
			{
				const auto& rangedComp = data.entity.getComponent<RangedEnemyComponent>();
				mSystemContext.eventManager.notify<SpawnProjectileEvent>(SpawnProjectileEvent(data.entity, rangedComp.cSpellId));
				return;
			}

			if (!data.lastAttackData)
				return;

			auto hitEntities = getHitEntities(data.entity, *data.lastAttackData);
			if (hitEntities.empty())
				return;

			mSystemContext.eventManager.notify<HitByAttackEvent>(HitByAttackEvent(data.entity, hitEntities));
		});
}

void AttackSystem::startAttack(Entity& entity)
{
	entity.getComponent<EntityStateComponent>().cCurrentState = EntityState::Attacking;

	if (entity.hasComponent<EntityAIStateComponent>())
		entity.getComponent<EntityAIStateComponent>().cState = EntityAIState::Attacking;

	auto animId = entity.getComponent<AttackComponent>().cNextAttackId;
	mSystemContext.eventManager.notify<PlayAttackAnimationEvent>(PlayAttackAnimationEvent(entity, animId));
	mFinishedEntities.push_back(&entity);
}

void AttackSystem::removeFinishedEntities()
{
	for (Entity* ent : mFinishedEntities)
		std::erase(mTrackedEntities, ent);
}

std::vector<Entity*> AttackSystem::getHitEntities(const Entity& entity, const AttackData& data) const
{
	auto dir = entity.getComponent<DirectionComponent>().cCurrentDir;
	if (!data.hitOffsets.contains(dir))
		return {};

	std::vector<Entity*> result;
	const auto& offsetsVector = data.hitOffsets.at(dir);
	bool isAttackerAPlayer = entity.hasComponent<PlayerComponent>();
	const auto attackerCell = Utilities::getEntityCell(entity);

	for (const auto& offset : offsetsVector)
	{
		const int nx = attackerCell.x + offset.x;
		const int ny = attackerCell.y + offset.y;

		auto entitiesVector = mTileMap.getEntitiesOnTile(nx, ny);
		for (Entity* ent : entitiesVector)
		{
			if (isAttackerAPlayer && ent->hasComponent<EnemyComponent>())
				result.push_back(ent);
			else if (!isAttackerAPlayer && ent->hasComponent<PlayerComponent>())
				result.push_back(ent);
		}
	}

	return result;
}
