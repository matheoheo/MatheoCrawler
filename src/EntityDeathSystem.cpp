#include "pch.h"
#include "EntityDeathSystem.h"
#include "Utilities.h"
#include "Random.h"
#include "MessageTypes.h"

EntityDeathSystem::EntityDeathSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
}

void EntityDeathSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mDeadEntities)
	{
		markEntityAsDead(*entity);
		mFinishedEntities.push_back(entity->getEntityId());
	}
	removeFinishedEntities();
}

void EntityDeathSystem::registerToEvents()
{
	mSystemContext.eventManager.registerEvent<EntityDiedEvent>([this](const EntityDiedEvent& data)
		{
			mDeadEntities.push_back(&data.entity);
			if (data.grantMoney)
			{
				addGoldToPlayer(data.entity);
				notifyUISystem();
			}
		});
}

void EntityDeathSystem::markEntityAsDead(Entity& entity)
{
	auto& evMng = mSystemContext.eventManager;
	auto& moveComp = entity.getComponent<MovementComponent>();

	evMng.notify<TileVacatedEvent>(TileVacatedEvent(entity, moveComp.cInitialPosition));
	evMng.notify<TileVacatedEvent>(TileVacatedEvent(entity, moveComp.cNextPos));
	evMng.notify<ReserveTileEvent>(ReserveTileEvent(nullptr, moveComp.cNextPos));
	evMng.notify<RemoveEntityFromSystemEvent>(RemoveEntityFromSystemEvent(entity));
}

void EntityDeathSystem::removeFinishedEntities()
{
	mDeadEntities.clear();
	for (const auto& id : mFinishedEntities)
	{
		mSystemContext.entityManager.removeEntity(id);
	}

	mFinishedEntities.clear();
}

int EntityDeathSystem::getGoldValue(Entity& entity) const
{
	const auto& combatStats = entity.getComponent<CombatStatsComponent>();
	constexpr int baseGold = 3;
	int entityPower = static_cast<int>(( (combatStats.cMaxHealth / 2)
		+ combatStats.cDefence * 2
		+ combatStats.cMagicDefence * 2)
		* (1 + combatStats.cAttackDamage / 19.f));

	int goldValue = baseGold * Config::difficulityLevel * entityPower;
	int minGold = static_cast<int>(goldValue * 0.9f);
	int maxGold = static_cast<int>(goldValue * 1.1f);
	return Random::get(minGold, maxGold);
}

void EntityDeathSystem::addGoldToPlayer(Entity& deadEntity)
{
	//right now, i am gonna keep this function here
	//as i do not have other resources for player
	//if the resources are gonna get bigger, i am gonna create a seperate system.
	auto& player = mSystemContext.entityManager.getPlayer();
	int gold = getGoldValue(deadEntity);
	player.getComponent<PlayerResourcesComponent>().cGold += gold;

	mSystemContext.eventManager.notify<LogMessageEvent>(LogMessageEvent(MessageType::GoldEarned, gold));
	mSystemContext.eventManager.notify<RecordStatisticEvent>(RecordStatisticEvent(StatisticType::GoldCollected, gold));
}

void EntityDeathSystem::notifyUISystem()
{
	mSystemContext.eventManager.notify<UpdatePlayerResourcesEvent>(UpdatePlayerResourcesEvent());
}