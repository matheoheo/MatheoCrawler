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
	//temporary

	static sf::Clock c;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U))
	{
		if (c.getElapsedTime().asMilliseconds() > 1000.f)
		{
			for (auto& e : mSystemContext.entityManager.getEntitiesWithComponents<EnemyComponent>())
			{
				mSystemContext.eventManager.notify<EntityDiedEvent>(EntityDiedEvent(*e));
			}
			c.restart();
		}
	}
}

void EntityDeathSystem::registerToEvents()
{
	mSystemContext.eventManager.registerEvent<EntityDiedEvent>([this](const EntityDiedEvent& data)
		{
			mDeadEntities.push_back(&data.entity);
			addGoldToPlayer(data.entity);
			notifyUISystem();
		});
}

void EntityDeathSystem::markEntityAsDead(Entity& entity)
{
	auto entityPos = Utilities::getEntityVisualPosition(entity);
	auto& evMng = mSystemContext.eventManager;

	evMng.notify<TileVacatedEvent>(TileVacatedEvent(entity, entityPos));
	evMng.notify<ReserveTileEvent>(ReserveTileEvent(nullptr, entityPos));
	evMng.notify<RemoveEntityFromSystemEvent>(RemoveEntityFromSystemEvent(entity));
}

void EntityDeathSystem::removeFinishedEntities()
{
	mDeadEntities.clear();
	for (const auto& id : mFinishedEntities)
	{
		Entity* ent = mSystemContext.entityManager.getEntity(id);
		if (ent)
		{
			ent->getComponent<SpriteComponent>().cSprite.setPosition({ -500.f, -500.f });
		}
	}
	mFinishedEntities.clear();
}

int EntityDeathSystem::getGoldValue(Entity& entity) const
{
	const auto& combatStats = entity.getComponent<CombatStatsComponent>();
	constexpr int baseGold = 2;
	int entityPower = ( (combatStats.cMaxHealth / 3)
		+ combatStats.cDefence * 2
		+ combatStats.cMagicDefence * 2)
		* (1 + combatStats.cAttackDamage / 20.f);

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
}

void EntityDeathSystem::notifyUISystem()
{
	mSystemContext.eventManager.notify<UpdatePlayerResourcesEvent>(UpdatePlayerResourcesEvent());
}