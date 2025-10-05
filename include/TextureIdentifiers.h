#pragma once

enum class TextureIdentifier
{
	//screens
	MattLogo,
	MenuBackground,
	OptionsBackground,
	CreditsBackground,
	AboutBackground,

	//entities
	Player,
	Skletorus,
	Bonvik,
	Moranna,
	Ray,

	//icons
	Suash, //gold icon
	Twarf, //player icon
	Attack1Icon,
	Attack2Icon,
	Attack3Icon,

	//Attributes (shop) icons
	HpIcon,
	AttDmgIcon,
	AttSpeedIcon,
	PhysDefIcon,
	MagDefIcon,

	//Healing Spells
	QuickHealIcon,
	MajorHealIcon,
	HealthRegenIcon,
	ManaRegenIcon,
	
	//Projectile Spells
	WaterballIcon,
	PureProjIcon,
	FireballIcon,
	BloodballIcon,

	//Aoe spells
	FrostPillarIcon,
	BladeDanceIcon,
	ToxicCloudIcon,
	ThunderstormIcon,

	//Effects icons
	FireBurnIcon,
	MoveSpeedSlowIcon,
	AttSpeedSlowIcon,
	MoveFrozenIcon,

	//Raw projectiles
	//They have '0', because the plan was to add more of those.
	WaterBall0,
	PureProjectile0,
	Fireball0,
	Bloodball0

};