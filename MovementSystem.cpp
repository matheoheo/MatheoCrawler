#include "pch.h"
#include "MovementSystem.h"
#include "Utilities.h"

MovementSystem::MovementSystem(SystemContext& systemContext)
	:ISystem(systemContext),
	mThreshold(3.0f)
{
	registerToEvents();
}

void MovementSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		if (isNearTargetPosition(*entity))
		{
			finishMovement(*entity);
			continue;
		}
		moveEntity(*entity, deltaTime);
	}
	eraseFinishedEntities();
}

void MovementSystem::registerToEvents()
{
	registerToMoveAllowedEvent();
}

void MovementSystem::moveEntity(const Entity& entity, const sf::Time& deltaTime)
{
	auto& spriteComp = entity.getComponent<SpriteComponent>();
	auto& moveComp = entity.getComponent<MovementComponent>();

	sf::Vector2f movementStep = moveComp.cDirectionVector *	moveComp.cMoveSpeed * deltaTime.asSeconds();
	spriteComp.cSprite.move(movementStep);
}

bool MovementSystem::isNearTargetPosition(const Entity& entity) const
{
	auto pos = entity.getComponent<SpriteComponent>().cSprite.getPosition();
	auto targetPos = entity.getComponent<MovementComponent>().cNextPos;

	return Utilities::getDistanceBetween(pos, targetPos) < mThreshold;
}

void MovementSystem::finishMovement(Entity& entity)
{
	auto& sprite = entity.getComponent<SpriteComponent>().cSprite;
	auto& moveComp = entity.getComponent<MovementComponent>();
	auto& stateComp = entity.getComponent<EntityStateComponent>();

	sprite.setPosition(moveComp.cNextPos);
	stateComp.cCurrentState = EntityState::Idle;

	if (entity.hasComponent<PlayerComponent>())
		mSystemContext.eventManager.notify<PlayerMoveFinishedEvent>(PlayerMoveFinishedEvent(moveComp.cNextPos));
	else
		mSystemContext.eventManager.notify<EntityMoveFinishedEvent>(EntityMoveFinishedEvent(entity, moveComp.cNextPos));

	mSystemContext.eventManager.notify<FinalizeAnimationEvent>(FinalizeAnimationEvent(entity));
	mSystemContext.eventManager.notify<ReserveTileEvent>(ReserveTileEvent(nullptr, moveComp.cNextPos));
	mSystemContext.eventManager.notify<TileOccupiedEvent>(TileOccupiedEvent(entity, moveComp.cNextPos));
	mSystemContext.eventManager.notify<TileVacatedEvent>(TileVacatedEvent(entity, moveComp.cInitialPosition));
}

void MovementSystem::eraseFinishedEntities()
{
	std::erase_if(mTrackedEntities, [](const Entity* entity)
		{
			return entity->getComponent<EntityStateComponent>().cCurrentState != EntityState::Moving;
		});
}

void MovementSystem::registerToMoveAllowedEvent()
{
	mSystemContext.eventManager.registerEvent<MoveAllowedEvent>([this](const MoveAllowedEvent& data)
		{
			auto& moveComp = data.entity.getComponent<MovementComponent>();
			auto& stateComp = data.entity.getComponent<EntityStateComponent>();
			auto& dirComp = data.entity.getComponent<DirectionComponent>();

			moveComp.cNextPos = data.nextPos;
			moveComp.cInitialPosition = data.entity.getComponent<SpriteComponent>().cSprite.getPosition();
			moveComp.cDirectionVector = Utilities::dirToVector(dirComp.cNextDir);
			stateComp.cCurrentState = EntityState::Moving;
			dirComp.cLastDir = dirComp.cCurrentDir;
			dirComp.cCurrentDir = dirComp.cNextDir;
			mTrackedEntities.push_back(&data.entity);

			mSystemContext.eventManager.notify<PlayGenericWalkEvent>(PlayGenericWalkEvent(data.entity));
			mSystemContext.eventManager.notify<ReserveTileEvent>(ReserveTileEvent(&data.entity, data.nextPos));
		});
}
