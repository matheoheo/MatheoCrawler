#pragma once
#include <SFML/System/Vector2.hpp>

namespace Config
{
	inline sf::Vector2u windowSize(1280, 800);
	inline sf::Vector2f fWindowSize(1280, 800.f);
	inline bool fullscreen = false;

	inline unsigned int getCharacterSize() { return windowSize.y / 17; }
	inline sf::Vector2f getCellSize() { return { 64.f, 64.f }; }

	inline const char delimiter = ':';

	inline constexpr sf::Color floorTileColor({ 180, 180, 160 });          // slight warm tint on visible floor
	inline constexpr sf::Color wallTileColor({ 90, 90, 80 });              // subtle warm gray on walls

	inline constexpr sf::Color dimmedFloorTileColor({ 110, 110, 90 });     // dim warm gray for explored floor
	inline constexpr sf::Color dimmedWallTileColor({ 50, 50, 40 });        // dim warm gray for walls

	//monsters bars color.
	inline constexpr sf::Color hpBarOutlineColor({60, 0, 0});
	inline constexpr sf::Color hpBarBackgroundColor({20, 20, 20});
	inline constexpr sf::Color hpBarForegroundColor({160, 30, 30});

	//player mana bar color
	inline constexpr sf::Color manaBarColor({ 122, 92, 255 });
	//player hp bar color
	inline constexpr sf::Color hpBarColor({ 0, 200, 0 });

	inline constexpr sf::Vector2f hpBarDefaultSize({ 50, 7 });

	inline sf::Vector2f hpBarPlayerSize({ 50, 7 });
	inline sf::Vector2f hpBarBossSize({ 50, 7 });
	inline sf::Vector2f manaBarSize({ 40, 5 });
	inline int difficulityLevel = 1;

	void loadConfiguration();
}