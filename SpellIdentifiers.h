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
	
	//Spells that are treated as basic attacks for ranged entities
	MorannaProjectile
};

enum class SpellType
{
	Heal,
	Projectile,
	AreaEffect,

	Count
};

enum class SpellEffect
{
	None,
	FireBurn,
	MovementSpeedSlow,
	AttackSpeedSlow,
	LifeSteal
};

