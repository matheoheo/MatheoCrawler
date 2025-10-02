#include "pch.h"
#include "ToxicCloudSpell.h"
#include "Random.h"
#include "Config.h"
#include "SpellHolder.h"
#include "Tile.h"
#include "EventManager.h"

ToxicCloudSpell::ToxicCloudSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos)
	:IAOESpell(caster, tileMap, castPos),
	mParticleSpawnTimer(0),
	mTimeSinceLastDamage(0)
{
	mCastTime = Timing::cloudLifetime;
	mCastPos = getCasterPos();
	spawnCloudOnStart();
}

void ToxicCloudSpell::update(const sf::Time& deltaTime, EventManager& eventManager)
{
	addTimePassed(deltaTime);
	if (hasCastFinished())
		complete();
	
	updateParticleSpawner(deltaTime);
	for (auto& effect : mToxicEffects)
		updateEffect(deltaTime, effect);
	updateDamageTick(deltaTime, eventManager);
}

void ToxicCloudSpell::render(sf::RenderWindow& window)
{
	for (const auto& effect : mToxicEffects)
		renderEffect(window, effect);
}

void ToxicCloudSpell::spawnCloudOnStart()
{
	const auto& offsets = SpellHolder::getInstance().get(SpellIdentifier::ToxicCloud).aoe->offsets;
	auto affectedTiles = getAffectedTiles(offsets, mCastPos);

	for (const Tile* tile : affectedTiles)
	{
		auto pos = tile->tileVisual.getPosition();
		spawnEffect(pos);
	}
}

void ToxicCloudSpell::updateParticleSpawner(const sf::Time& deltaTime)
{
	mParticleSpawnTimer += deltaTime.asMilliseconds();
	if (mParticleSpawnTimer >= Timing::particleSpawnInterval)
	{
		for (auto& effect : mToxicEffects)
			spawnParticle(effect);
		mParticleSpawnTimer = 0;
	}
}

void ToxicCloudSpell::spawnEffect(const sf::Vector2f& pos)
{
	constexpr int numBaseCloudShapes = 4;
	ToxicCloudEffect effect;
	effect.tilePos = pos;
	for (int i = 0; i < numBaseCloudShapes; ++i)
	{
		float radius = getRandomBaseCircleRadius();
		effect.baseCircles.emplace_back(radius);
		effect.baseCircles.back().setOrigin({ radius, radius });
		effect.baseCircles.back().setFillColor(getRandomBaseCircleColor());
		effect.baseCircles.back().setPosition(getRandomPosInsideTile(pos, radius));
	}
	mToxicEffects.push_back(std::move(effect));
}

void ToxicCloudSpell::spawnParticle(ToxicCloudEffect& effect)
{
	ToxicCloudEffect::Particle particle;
	particle.lifeTime = getRandomParticleLifetime();
	float randomSpeed = getRandomParticleSpeed();
	sf::Vector2f vel = getRandomVelocity();
	particle.direction = vel * randomSpeed;
	float radius = getRandomParticleRadius();
	particle.shape.setRadius(radius);
	particle.shape.setFillColor(getRandomParticleColor());
	particle.shape.setPosition(getRandomPosInsideTile(effect.tilePos, radius));

	effect.timeSinceLastParticleSpawn = 0;
	effect.particles.push_back(std::move(particle));
}

void ToxicCloudSpell::renderEffect(sf::RenderWindow& window, const ToxicCloudEffect& effect)
{
	for (const auto& base : effect.baseCircles)
		window.draw(base);
	for (const auto& particle : effect.particles)
		window.draw(particle.shape);
}

void ToxicCloudSpell::updateEffect(const sf::Time& deltaTime, ToxicCloudEffect& effect)
{
	effect.timeSinceLastParticleSpawn += deltaTime.asMilliseconds();
	for (auto& particle : effect.particles)
	{
		particle.timeSinceSpawn += deltaTime.asMilliseconds();
		particle.shape.move(particle.direction * deltaTime.asSeconds());
	}

	std::erase_if(effect.particles, [](const ToxicCloudEffect::Particle& particle)
		{
			return particle.timeSinceSpawn >= particle.lifeTime;
		});
}

float ToxicCloudSpell::getRandomBaseCircleRadius() const
{
	return static_cast<float>(Random::get(Visuals::baseCircleRadiusMin, Visuals::baseCircleRadiusMax));
}

float ToxicCloudSpell::getRandomParticleRadius() const
{
	return static_cast<float>(Random::get(Visuals::particleRadiusMin, Visuals::particleRadiusMax));
}

int ToxicCloudSpell::getRandomParticleLifetime() const
{
	return Random::get(Timing::particleLifetimeMin, Timing::particleLifetimeMax);
}

float ToxicCloudSpell::getRandomParticleSpeed() const
{
	return static_cast<float>(Random::get(Visuals::particleSpeedMin, Visuals::particleSpeedMax));
}

sf::Color ToxicCloudSpell::getRandomBaseCircleColor() const
{
	static constexpr std::array<sf::Color, 4> availableColors =
	{
		Colors::darkGreen, Colors::mossGreen, Colors::toxicGreen, Colors::limeFog
	};
	int randomIndex = Random::get(0, availableColors.size() - 1);
	return availableColors[randomIndex];
}

sf::Color ToxicCloudSpell::getRandomParticleColor() const
{
	static constexpr std::array<sf::Color, 3> availableColors =
	{
		Colors::lightLime, Colors::paleMist, Colors::acidGlow
	};
	int randomIndex = Random::get(0, availableColors.size() - 1);
	return availableColors[randomIndex];
}

sf::Vector2f ToxicCloudSpell::getRandomPosInsideTile(const sf::Vector2f& pos, float radius) const
{
	int iRadius = static_cast<int>(radius);

	int minX = static_cast<int>(pos.x) + iRadius;
	int minY = static_cast<int>(pos.y) + iRadius;

	int maxX = minX + static_cast<int>(Config::getCellSize().x) - iRadius;
	int maxY = minY + static_cast<int>(Config::getCellSize().y) - iRadius;

	return sf::Vector2f
	{
		static_cast<float>(Random::get(minX, maxX)),
		static_cast<float>(Random::get(minY, maxY))
	};
}

sf::Vector2f ToxicCloudSpell::getRandomVelocity() const
{
	constexpr float y = -1.f; //always moves up
	constexpr int minXVel = -20;
	constexpr int maxXVel = 20;
	int xVel = Random::get(minXVel, maxXVel);
	float x = static_cast<float>(xVel);
	x /= 100.f; //to get value between [-0.2, 0.2]

	return sf::Vector2f{ x, y };
}

void ToxicCloudSpell::updateDamageTick(const sf::Time& deltaTime, EventManager& eventManager)
{
	mTimeSinceLastDamage += deltaTime.asMilliseconds();
	if (mTimeSinceLastDamage < Timing::damageTickTime)
		return;

	const auto& spell = SpellHolder::getInstance().get(SpellIdentifier::ToxicCloud);
	const auto& offsets = spell.aoe->offsets;
	auto affectedEntities = getAffectedEntities(offsets, mCastPos);
	IAOESpell::hitEntities(affectedEntities, SpellIdentifier::ToxicCloud, eventManager);
	
	mTimeSinceLastDamage = 0;
}
