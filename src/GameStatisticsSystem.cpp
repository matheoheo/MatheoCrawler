#include "pch.h"
#include "GameStatisticsSystem.h"

GameStatisticsSystem::GameStatisticsSystem(SystemContext& sysContext)
	:ISystem(sysContext),
	mPlayerAlive(true)
{
	registerToEvents();
	initStatistics();
}

void GameStatisticsSystem::update(const sf::Time& deltaTime)
{
	if (mPlayerAlive)
		mStats[StatisticType::TimeSurvived].value += static_cast<int>(deltaTime.asMilliseconds());

	static sf::Clock c;
	if (c.getElapsedTime().asMilliseconds() > 1000)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::U))
		{
			mSystemContext.eventManager.notify<PlayerRunEndedEvent>(PlayerRunEndedEvent(mStats));
			c.restart();
		}
	}
}

void GameStatisticsSystem::registerToEvents()
{
	registerToRecordStatisticEvent();
	registerToPlayerDiedEvent();
}

void GameStatisticsSystem::registerToRecordStatisticEvent()
{
	mSystemContext.eventManager.registerEvent<RecordStatisticEvent>([this](const RecordStatisticEvent& data)
		{
			add(data.type, data.value);
		});
}

void GameStatisticsSystem::registerToPlayerDiedEvent()
{
	mSystemContext.eventManager.registerEvent<PlayerDiedEvent>([this](const PlayerDiedEvent& data)
		{
			mPlayerAlive = false;
			mSystemContext.eventManager.notify<PlayerRunEndedEvent>(PlayerRunEndedEvent(mStats));
		});
}

void GameStatisticsSystem::initStatistics()
{
	mStats[StatisticType::GoldCollected]    = { "Gold Collected", 100 };
	mStats[StatisticType::TotalDamageDealt] = { "Damage Dealt", 0 };
	mStats[StatisticType::TotalDamageTaken] = { "Damage Taken", 0 };
	mStats[StatisticType::EnemiesDefeated]  = { "Enemies Defeated", 0 };
	mStats[StatisticType::StepsTaken]       = { "Steps Taken", 0 };
	mStats[StatisticType::HitsTaken]        = { "Hits Taken", 0 };
	mStats[StatisticType::HitsDealt]        = { "Hits Dealt", 0 };
	mStats[StatisticType::TimeSurvived]		= { "Time Survived", 0 };
}

void GameStatisticsSystem::add(StatisticType type, int value)
{
	if (!mStats.contains(type))
		return;

	mStats.at(type).value += value;
}
