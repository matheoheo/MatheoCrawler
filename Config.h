#pragma once
#include <SFML/System/Vector2.hpp>

namespace Config
{
	inline sf::Vector2u windowSize(1280, 768);
	inline sf::Vector2f fWindowSize(1280.f, 768.f);

	inline unsigned int getCharacterSize() { return windowSize.y / 17; }
	inline sf::Vector2f getCellSize() { return { 64.f, 64.f }; }

	inline const char delimiter = ':';
	/*
	inline constexpr sf::Color floorTileColor({130, 130, 130});
	inline constexpr sf::Color wallTileColor({50, 50, 50});
	inline constexpr sf::Color dimmedFloorTileColor({ 60, 60, 60 });
	inline constexpr sf::Color dimmedWallTileColor({ 20, 20, 20 });
	
	
	inline constexpr sf::Color floorTileColor({ 180, 180, 180 });          // lighter gray for visible floor
	inline constexpr sf::Color wallTileColor({ 80, 80, 80 });              // medium-dark gray for visible walls

	inline constexpr sf::Color dimmedFloorTileColor({ 100, 100, 100 });    // darker gray but not too dark for explored floors
	inline constexpr sf::Color dimmedWallTileColor({ 40, 40, 40 });        // very dark gray for explored walls
	*/

	inline constexpr sf::Color floorTileColor({ 180, 180, 160 });          // slight warm tint on visible floor
	inline constexpr sf::Color wallTileColor({ 90, 90, 80 });              // subtle warm gray on walls

	inline constexpr sf::Color dimmedFloorTileColor({ 110, 110, 90 });     // dim warm gray for explored floor
	inline constexpr sf::Color dimmedWallTileColor({ 50, 50, 40 });        // dim warm gray for walls
	inline int difficulityLevel = 1;
}