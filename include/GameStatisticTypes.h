#pragma once

enum class StatisticType
{
	GoldCollected,
	TotalDamageDealt,
	TotalDamageTaken,
	EnemiesDefeated,
	StepsTaken,
	HitsDealt,
	HitsTaken,
	TimeSurvived
};


struct Statistic {
	std::string label;
	int value = 0;
};
using StatisticMap = std::unordered_map<StatisticType, Statistic>;
