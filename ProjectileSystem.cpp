#include "pch.h"
#include "ProjectileSystem.h"
#include "Utilities.h"
#include "TileMap.h"
#include "Entity.h"
#include "Random.h"

ProjectileSystem::ProjectileSystem(SystemContext& systemContext, TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void ProjectileSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		moveProjectile(*entity, deltaTime);
		if (hasHitSomeone(*entity))
		{
			handleEntityHit(*entity);
		}
		if (hasExceededRange(*entity) || hasHitWall(*entity))
		{
			markAsFinished(entity->getEntityId());
		}
	}

	removeFinished();
}

void ProjectileSystem::render(sf::RenderWindow& window)
{
	for (const auto& entity : mTrackedEntities)
	{
		auto& sprite = entity->getComponent<SpriteComponent>().cSprite;
		window.draw(sprite);
	}
}

void ProjectileSystem::registerToEvents()
{
	registerToProjectileSpawnedEvent();
}

void ProjectileSystem::registerToProjectileSpawnedEvent()
{
	mSystemContext.eventManager.registerEvent<ProjectileSpawnedEvent>([this](const ProjectileSpawnedEvent& data)
		{
			mTrackedEntities.push_back(&data.entity);
		});
}

void ProjectileSystem::moveProjectile(const Entity& entity, const sf::Time& deltaTime)
{
	//we do not send projectiles to MovementSystem, because that system supports tile to tile movement
	//with tile reservations and occupation, but projectiles move more freely
	auto& moveComp = entity.getComponent<MovementComponent>();
	auto& sprite = entity.getComponent<SpriteComponent>().cSprite;
	auto& projComp = entity.getComponent<SpellProjectileComponent>();

	const sf::Vector2f moveVector{moveComp.cDirectionVector * moveComp.cMoveSpeed * deltaTime.asSeconds()};
	const float movedDistance = std::abs(moveVector.x) + std::abs(moveVector.y);
	sprite.move(moveVector);

	projComp.cDistanceTraveled += movedDistance;
}

bool ProjectileSystem::hasExceededRange(const Entity& entity)
{
	auto& projComp = entity.getComponent<SpellProjectileComponent>();
	return projComp.cDistanceTraveled > projComp.cMaxDistance;
}

bool ProjectileSystem::hasHitSomeone(const Entity& projectile)
{
	auto cellIndex = Utilities::getEntityCellRaw(projectile);
	return mTileMap.isTileOccupied(cellIndex.x, cellIndex.y);
}

void ProjectileSystem::handleEntityHit(const Entity& projectile)
{
	auto cellIndex = Utilities::getEntityCellRaw(projectile);
	auto hitEntities = mTileMap.getEntitiesOnTile(cellIndex.x, cellIndex.y);
	auto& projComp = projectile.getComponent<SpellProjectileComponent>();
	bool playerCasted = projComp.cPlayerCasted;

	for (Entity* ent : hitEntities)
	{
		bool isEntPlayer = ent->hasComponent<PlayerComponent>();
		//if caster is a player, and hit entity is a player - continue
		//if caster is enemy and hit entity is enemy - continue
		//if target was already hit by this projectile - continue
		if ((playerCasted && isEntPlayer) || (!playerCasted && !isEntPlayer) || wasAlreadyHit(*ent, projComp))
			continue;

		onHit(*ent, projectile.getEntityId(), projComp, isEntPlayer);
	}
}

void ProjectileSystem::addEntityTargetHit(const Entity& hit, SpellProjectileComponent& projComp)
{
	projComp.cHitTargets.emplace_back(hit.getEntityId());
}

void ProjectileSystem::onHit(Entity& hitEntity, Entity::EntityID projId, SpellProjectileComponent& projComp, bool wasPlayerHit)
{
	if (!projComp.cSpellData->pierce)
		markAsFinished(projId);
	else
	{
		addEntityTargetHit(hitEntity, projComp);
		auto hitTargetsSize = projComp.cHitTargets.size();
		if (hitTargetsSize >= projComp.cSpellData->maxTargets)
			markAsFinished(projId);
	}
	int projDmg = projComp.cFinalDmg;
	bool playerCasted = projComp.cPlayerCasted;
	mSystemContext.eventManager.notify<HitByProjectileEvent>(HitByProjectileEvent(hitEntity, projDmg, wasPlayerHit, playerCasted));
}

bool ProjectileSystem::hasHitWall(const Entity& projectile) const
{
	auto cellIndex = Utilities::getEntityCellRaw(projectile);
	return mTileMap.isTileSolid(cellIndex.x, cellIndex.y);
}

bool ProjectileSystem::wasAlreadyHit(const Entity& entity, SpellProjectileComponent& projComp)
{
	return std::ranges::find(projComp.cHitTargets, entity.getEntityId()) != std::ranges::end(projComp.cHitTargets);
}

void ProjectileSystem::markAsFinished(Entity::EntityID id)
{
	mFinishedProjectiles.emplace_back(id);
}

void ProjectileSystem::removeFinished()
{
	std::erase_if(mTrackedEntities, [this](const Entity* ent)
		{
			return std::ranges::find(mFinishedProjectiles, ent->getEntityId()) != std::ranges::end(mFinishedProjectiles);
		});
	mFinishedProjectiles.clear();
}
