#include "pch.h"
#include "PatrolAISystem.h"
#include "Components.h"
#include "Utilities.h"
#include "Random.h"

PatrolAISystem::PatrolAISystem(SystemContext& systemContext, const TileMap& tileMap)
	:ISystem(systemContext),
	mTileMap(tileMap)
{
	registerToEvents();
}

void PatrolAISystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		if (!Utilities::isEntityIdling(*entity))
			continue;

		auto& patrolComp = entity->getComponent<PatrolAIComponent>();
		addToPatrolTimer(patrolComp, static_cast<float>(deltaTime.asMilliseconds()));

		if (isTimeForNextMove(patrolComp))
		{
			doPatrol(*entity);
			resetPatrolTimer(patrolComp);
		}
	}

	removeNonPatrollingEntities();
}

bool PatrolAISystem::isTimeForNextMove(const PatrolAIComponent& patrolComp) const
{
	return patrolComp.cElapsed > patrolComp.cPatrolCooldown;
}

void PatrolAISystem::addToPatrolTimer(PatrolAIComponent& patrolComp, float elapsed) const
{
	patrolComp.cElapsed += elapsed;
}

void PatrolAISystem::resetPatrolTimer(PatrolAIComponent& patrolComp) const
{
	patrolComp.cElapsed = 0.f;
}

void PatrolAISystem::doPatrol(Entity& entity)
{
	auto nextDir = getRandomValidDirection(entity);
	if (!nextDir)
		return;

	mSystemContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(entity, nextDir.value()));
}

std::optional<Direction> PatrolAISystem::getRandomValidDirection(const Entity& entity) const
{
	auto pos = Utilities::getEntityPos(entity);
	auto index = Utilities::getCellIndex(pos);
	constexpr std::array<std::pair<Direction, sf::Vector2i>, 4> offsets =
	{ {
		{Direction::Up,     {0, -1}}, 
		{Direction::Left,   {-1, 0}},
		{Direction::Bottom, {0, 1}}, 
		{Direction::Right,  {1, 0}} 
	} };
	
	std::vector<Direction> candidates;
	for (const auto& offset : offsets)
	{
		int nx = index.x + offset.second.x;
		int ny = index.y + offset.second.y;
		
		if (mTileMap.isTileWalkable(nx, ny))
			candidates.emplace_back(offset.first);
	}
	if (candidates.empty())
		return std::nullopt;

	if (candidates.size() == 1)
		return candidates[0];

	int randomIndex = Random::get(0, static_cast<int>(candidates.size() - 1));
	return candidates[randomIndex];
}

void PatrolAISystem::registerToEvents()
{
	mSystemContext.eventManager.registerEvent<StartPatrollingEvent>([this](const StartPatrollingEvent& data)
		{
			auto& aiStateComp = data.entity.getComponent<EntityAIStateComponent>();
			//if (aiStateComp.cState == EntityAIState::Patrolling)
			//	return;

			if (!isEntityAlreadyTracked(data.entity))
			{
				aiStateComp.cState = EntityAIState::Patrolling;
				mTrackedEntities.push_back(&data.entity);
			}
		});
}

void PatrolAISystem::removeNonPatrollingEntities()
{
	std::erase_if(mTrackedEntities, [this](const Entity* ent)
		{
			return ent->getComponent<EntityAIStateComponent>().cState != EntityAIState::Patrolling;
		});
}
