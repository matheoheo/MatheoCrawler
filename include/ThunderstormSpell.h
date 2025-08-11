#pragma once
#include "IAOESpell.h"

class ThunderstormSpell
	:public IAOESpell
{
public:
	ThunderstormSpell(const Entity& caster, const TileMap& tileMap, const sf::Vector2f& castPos);
	// Inherited via IAOESpell
	virtual void update(const sf::Time& deltaTime, EventManager& eventManager) override;
	virtual void render(sf::RenderWindow& window) override;
private:
	struct ThunderstormConfig
	{
		struct Timing {
			static constexpr int spellDuration = 1500;
			static constexpr int warningDuration = 600;
			static constexpr int strikeBoltDuration = 900;
			static constexpr int boltFlashDuration = 200;
		};
		struct Colors {
			static constexpr sf::Color startWarningColor{ 50, 100, 255, 120 };
			static constexpr sf::Color targetWarningColor{ 150, 220, 255, 200 };
			static constexpr sf::Color strikeBoltColor{ 80, 120, 255, 0 }; //alpha =0 , because we will be fading in.
		};
		struct Visuals {
			static constexpr sf::Vector2f startWarningSize{ 2.f, 2.f };
			static constexpr int strikeBoltSegments = 4;
		};
	};
	using Timing = ThunderstormConfig::Timing;
	using Colors = ThunderstormConfig::Colors;
	using Visuals = ThunderstormConfig::Visuals;

private:
	struct Warning
	{
		sf::Vector2f tilePos;
		sf::RectangleShape shape;
	};
	void createOnStartEffect();
	void createWarning(const sf::Vector2f& pos);
	void updateWarnings();
	float getWarningProgress() const;
private:
	struct StrikeBolt
	{
		std::vector<sf::VertexArray> thickBolts;
	};
	void createBolt(const sf::Vector2f& pos);
	void updateBolts();
	void renderBolts(sf::RenderWindow & window);
private:
	void makeDamage(EventManager& eventManager);
private:
	std::vector<Warning> mWarnings; //shapes that will communicate visually, that a spell is being casted.
	std::vector<StrikeBolt> mStrikeBolts;
};