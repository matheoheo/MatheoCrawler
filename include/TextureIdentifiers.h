#pragma once

enum class TextureIdentifier
{
	//screens
	MattLogo,
	MenuBackground,
	OptionsBackground,

	//entities
	Player,
	Skletorus,
	Bonvik,
	Moranna,

	//icons
	Suash, //gold icon
	Twarf, //player icon
	Attack1Icon,
	Attack2Icon,
	Attack3Icon,

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