#include "pch.h"
#include "TileFadeSystem.h"
#include "Utilities.h"

TileFadeSystem::TileFadeSystem(SystemContext& systemContext)
	:ISystem(systemContext)
{
	registerToEvents();
}

void TileFadeSystem::update(const sf::Time& deltaTime)
{
	for (Tile* tile : mFadingTiles)
	{
		if (tile->visible)
		{
			makeVisibleFade(*tile);
			markTileAsFinished(*tile);
			continue;
		}
		else if (tile->explored)
		{
			bool sf = shouldKeepFading(*tile);
			if (sf)
				fadeTile(*tile);
			else
			{
				makeExploredFade(*tile);
				markTileAsFinished(*tile);
			}
		}

	}
	removeFinishedTiles();
}

void TileFadeSystem::registerToEvents()
{
	registerToTileFadeRequestEvent();
}

void TileFadeSystem::registerToTileFadeRequestEvent()
{
	mSystemContext.eventManager.registerEvent<TileFadeRequestEvent>([this](const TileFadeRequestEvent& data)
		{
			mFadingTiles.insert(std::begin(data.tiles), std::end(data.tiles));
		});
}

void TileFadeSystem::removeFinishedTiles()
{
	std::erase_if(mFadingTiles, [this](const Tile* tile)
		{
			return std::ranges::find(mFinishedFadingTiles, tile) != std::ranges::end(mFinishedFadingTiles);
		});
	mFinishedFadingTiles.clear();
}

void TileFadeSystem::setTileColor(Tile& tile, const sf::Color& color)
{
	tile.tileVisual.setFillColor(color);
}

void TileFadeSystem::markTileAsFinished(Tile& tile)
{
	mFinishedFadingTiles.push_back(&tile);
}

void TileFadeSystem::makeVisibleFade(Tile& tile)
{
	const auto& color = Utilities::getVisibleTileColor(tile);
	setTileColor(tile, color);
}

void TileFadeSystem::makeExploredFade(Tile& tile)
{
	const auto& color = Utilities::getExploredTileColor(tile);
	setTileColor(tile, color);
}

bool TileFadeSystem::shouldKeepFading(Tile& tile) const
{
	const auto& targetColor = Utilities::getExploredTileColor(tile);
	const auto tileColor = tile.tileVisual.getFillColor();

	return !isColorCloseEnough(tileColor, targetColor);
}

void TileFadeSystem::fadeTile(Tile& tile)
{
	const auto& targetColor = Utilities::getExploredTileColor(tile);
	const auto tileColor = tile.tileVisual.getFillColor();
	const float lerpTransition = 0.03f;

	const auto nextColor = lerpColor(tileColor, targetColor, lerpTransition);
	setTileColor(tile, nextColor);
}

sf::Color TileFadeSystem::lerpColor(const sf::Color& startColor, const sf::Color& endColor, float t) const
{
	return sf::Color
	{
		static_cast<std::uint8_t>(startColor.r + (endColor.r - startColor.r) * t),
		static_cast<std::uint8_t>(startColor.g + (endColor.g - startColor.g) * t),
		static_cast<std::uint8_t>(startColor.b + (endColor.b - startColor.b) * t),
		startColor.a
	};
}

bool TileFadeSystem::isColorCloseEnough(const sf::Color& currColor, const sf::Color& targetColor, int threshold) const
{
	return 
		(std::abs(currColor.r - targetColor.r) <= threshold) &&
		(std::abs(currColor.g - targetColor.g) <= threshold) &&
		(std::abs(currColor.b - targetColor.b) <= threshold);
}
