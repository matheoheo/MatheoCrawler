#include "pch.h"
#include "BarRenderSystem.h"
#include "Utilities.h"
#include "Entity.h"

BarRenderSystem::BarRenderSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
}

void BarRenderSystem::update(const sf::Time& deltaTime)
{
	constexpr int maxVisibleTime = 1700; //milliseconds (1.7s)
	for (const auto& entity : mTrackedEntities)
	{
		auto& hpBarComp = entity->getComponent<HealthBarComponent>();
		hpBarComp.cVisibleTimer += deltaTime.asMilliseconds();
		if (hpBarComp.cVisibleTimer >= maxVisibleTime)
		{
			hpBarComp.cIsVisible = false;
			continue;
		}
		updateBarPosition(*entity);
	}

	removeFinishedEntities();
}

void BarRenderSystem::render(sf::RenderWindow& window)
{
	for (const auto& entity : mTrackedEntities)
	{
		const auto& hpBarComp = entity->getComponent<HealthBarComponent>();
		if (hpBarComp.cIsVisible)
		{
			window.draw(hpBarComp.cBackgroundBar);
			window.draw(hpBarComp.cForegroundBar);
		}
	}
}

void BarRenderSystem::registerToEvents()
{
	registerToUpdateHealthBarEvent();
}

void BarRenderSystem::registerToUpdateHealthBarEvent()
{
	mSystemContext.eventManager.registerEvent<HealthBarUpdateEvent>([this](const HealthBarUpdateEvent& data)
		{
			if (data.entity.hasComponent<PlayerComponent>())
				return;

			auto& hpBarComp = data.entity.getComponent<HealthBarComponent>();
			hpBarComp.cVisibleTimer = 0;
			hpBarComp.cIsVisible = true;
			sf::Vector2f newBarSize;

			if (data.entity.hasComponent<PlayerComponent>())
				newBarSize = calculateNewBarSize(data.entity, Config::hpBarPlayerSize);
			else if (data.entity.hasComponent<EnemyComponent>())
				newBarSize = calculateNewBarSize(data.entity, Config::hpBarDefaultSize);
			else //boss case
				newBarSize = calculateNewBarSize(data.entity, Config::hpBarBossSize);

			hpBarComp.cForegroundBar.setSize(newBarSize);

			if (isEntityAlreadyTracked(data.entity))
				return;

			mTrackedEntities.push_back(&data.entity);
		});
}

sf::Vector2f BarRenderSystem::calculateNewBarSize(const Entity& entity, const sf::Vector2f& originalSize) const
{
	return Utilities::calculateNewBarSize(entity, originalSize);
}

void BarRenderSystem::updateBarPosition(const Entity& entity)
{
	auto& hpBarComp = entity.getComponent<HealthBarComponent>();
	auto entCell = Utilities::getEntityCell(entity);
	sf::Vector2f cellPos = { static_cast<float>(entCell.x) * Config::getCellSize().x, static_cast<float>(entCell.y) * Config::getCellSize().y };
	constexpr sf::Vector2f offset = { 7, 2 };
	sf::Vector2f barPos = cellPos + offset;

	hpBarComp.cBackgroundBar.setPosition(barPos);
	hpBarComp.cForegroundBar.setPosition(barPos + sf::Vector2f{1.f, 1.f});
}

void BarRenderSystem::removeFinishedEntities()
{
	std::erase_if(mTrackedEntities, [](const Entity* ent)
		{
			return ent->getComponent<HealthBarComponent>().cIsVisible == false;
		});
}
