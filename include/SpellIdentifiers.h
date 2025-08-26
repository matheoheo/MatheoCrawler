#pragma once

enum class SpellIdentifier
{
	//Healing Spells
	QuickHeal,
	MajorHeal,
	HealthRegen,
	ManaRegen,

	//Projectile Spells
	WaterBall,
	PureProjectile,
	Fireball,
	Bloodball,
	
	//AOE
	FrostPillar,
	BladeDance,
	ToxicCloud,
	Thunderstorm,

	//Spells that are treated as basic attacks for ranged entities
	MorannaProjectile
};

enum class SpellType
{
	Regen,
	Heal,
	Projectile,
	AreaOfEffect,

	Count
};

enum class SpellEffect
{
	None,
	FireBurn,
	MovementSpeedSlow,
	AttackSpeedSlow,
	LifeSteal,
	MovementFrozen
};

