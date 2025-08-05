#include "IAOESpell.h"


class ToxicCloudSpell : public IAOESpell
{
public:
	ToxicCloudSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos);
	// Inherited via IAOESpell
	virtual void update(const sf::Time& deltaTime, EventManager& eventManager) override;
	virtual void render(sf::RenderWindow& window) override;
private:
	struct ToxicCloudConfig
	{
		struct Timing{
			static constexpr int cloudLifetime = 5500; //Total duration of the cloud in milliseconds
			static constexpr int damageTickTime = 1000; //time between damage ticks
			static constexpr int particleSpawnInterval = 300; //time between spawning particles
			static constexpr int particleLifetimeMin = 600; //in ms
			static constexpr int particleLifetimeMax = 1300;
		};
		struct Colors {
			static constexpr sf::Color darkGreen{ 50, 120, 50, 140 };
			static constexpr sf::Color mossGreen{ 80, 170, 80, 160 };
			static constexpr sf::Color toxicGreen{ 100, 150, 50, 150 };
			static constexpr sf::Color limeFog{ 120, 220, 120, 130 };
			static constexpr sf::Color lightLime{ 155, 255, 150, 140 };
			static constexpr sf::Color paleMist{ 200, 255, 200, 130 };
			static constexpr sf::Color acidGlow{ 180, 240, 120, 130 };
		};
		struct Visuals {
			static constexpr int baseCircleRadiusMin = 12;
			static constexpr int baseCircleRadiusMax = 18;
			static constexpr int particleRadiusMin = 5;
			static constexpr int particleRadiusMax = 9;
			static constexpr int particleSpeedMin = 20;
			static constexpr int particleSpeedMax = 40;
		};
	};
	using Timing = ToxicCloudConfig::Timing;
	using Colors = ToxicCloudConfig::Colors;
	using Visuals = ToxicCloudConfig::Visuals;

	struct ToxicCloudEffect
	{
		struct Particle
		{
			sf::CircleShape shape;
			sf::Vector2f direction;
			int lifeTime = 0;
			int timeSinceSpawn = 0;
		};

		std::vector<sf::CircleShape> baseCircles;
		std::vector<Particle> particles;
		sf::Vector2f tilePos;
		int timeSinceLastParticleSpawn = 0;
	};

	void spawnCloudOnStart();
	void updateParticleSpawner(const sf::Time& deltaTime);
	void spawnEffect(const sf::Vector2f& pos);
	void spawnParticle(ToxicCloudEffect& effect);
	void renderEffect(sf::RenderWindow& window, const ToxicCloudEffect& effect);
	void updateEffect(const sf::Time& deltaTime, ToxicCloudEffect& effect);

	float getRandomBaseCircleRadius() const;
	float getRandomParticleRadius() const;
	float getRandomParticleLifetime() const;
	float getRandomParticleSpeed() const;
	sf::Color getRandomBaseCircleColor() const;
	sf::Color getRandomParticleColor() const;
	sf::Vector2f getRandomPosInsideTile(const sf::Vector2f& pos, float radius) const;
	sf::Vector2f getRandomVelocity() const;

	void updateDamageTick(const sf::Time& deltaTime, EventManager& eventManager);
private:
	std::vector<ToxicCloudEffect> mToxicEffects;
	int mParticleSpawnTimer;
	int mTimeSinceLastDamage;
};