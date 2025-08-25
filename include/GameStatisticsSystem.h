#pragma once
#include "ISystem.h"
#include "GameStatisticTypes.h"

class GameStatisticsSystem 
	: public ISystem
{
public:
	GameStatisticsSystem(SystemContext& sysContext);
	virtual void update(const sf::Time& deltaTime) override;
public:

private:
	void registerToEvents();
	void registerToRecordStatisticEvent();
	void registerToPlayerDiedEvent();
	void initStatistics();
	void add(StatisticType type, int value);
private:
	void calculateAvgDamage();
private:
	StatisticMap mStats;
	bool mPlayerAlive;
};