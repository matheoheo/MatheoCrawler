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
	TimeSurvived,
	AvgDamageDealt,
	AvgDamageTaken
};


struct Statistic {
	std::string label;
	int value = 0;
	std::optional<StatisticType> type = std::nullopt;
};
using StatisticMap = std::unordered_map<StatisticType, Statistic>;
