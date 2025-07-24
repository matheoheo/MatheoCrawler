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
	Attack1Icon, //attack1 icon
	Attack2Icon,
	Attack3Icon,
	QuickHealIcon,
	MajorHealIcon,
	HealthRegenIcon,
	ManaRegenIcon,
	WaterballIcon,
	PureProjIcon,
	FireballIcon,
	BloodballIcon,
	FireBurnIcon,
	MoveSpeedSlowIcon,
	AttSpeedSlowIcon,

	//Raw projectiles
	//They have '0', because the plan was to add more of those.
	WaterBall0,
	PureProjectile0,
	Fireball0,
	Bloodball0

};