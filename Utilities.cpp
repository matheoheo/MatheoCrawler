#include "pch.h"
#include "Utilities.h"
#include "Config.h"
#include "Entity.h"
#include "EntityStates.h"
#include "TileMap.h"
#include "Directions.h"
#include "SpellIdentifiers.h"
#include "AnimationIdentifiers.h"

int Utilities::getDistanceBetweenCells(const sf::Vector2i& cellA, const sf::Vector2i& cellB)
{
	return std::abs(cellA.x - cellB.x) + std::abs(cellA.y - cellB.y);
}

float Utilities::getDistanceBetween(const sf::Vector2f& pointA, const sf::Vector2f& pointB)
{
	return std::hypotf(pointA.x - pointB.x, pointA.y - pointB.y);
}

float Utilities::getDistanceBetween(const sf::Vector2i& pointA, const sf::Vector2i& pointB)
{
	const sf::Vector2f fA = { static_cast<float>(pointA.x), static_cast<float>(pointA.y) };
	const sf::Vector2f fB = { static_cast<float>(pointB.x), static_cast<float>(pointB.y) };

	return getDistanceBetween(fA, fB);
}

sf::Vector2i Utilities::getCellIndex(const sf::Vector2f& pos)
{
	int cellX = static_cast<int>(pos.x) / static_cast<int>(Config::getCellSize().x);
	int cellY = static_cast<int>(pos.y) / static_cast<int>(Config::getCellSize().y);
	return { cellX, cellY };
}

sf::Vector2i Utilities::getNextCellIndex(const sf::Vector2i& currentCell, Direction dir)
{
	if (dir == Direction::Up)
		return currentCell + sf::Vector2i{ 0, -1 };
	else if (dir == Direction::Bottom)
		return currentCell + sf::Vector2i{ 0, 1 };
	else if (dir == Direction::Left)
		return currentCell + sf::Vector2i{ -1,0 };
	else if (dir == Direction::Right)
		return currentCell + sf::Vector2i{ 1, 0 };

	return currentCell;
}

sf::Vector2f Utilities::getEntityPos(const Entity& entity)
{
	return entity.getComponent<SpriteComponent>().cSprite.getPosition();
}

sf::Vector2f Utilities::getEntityVisualPosition(const Entity& entity)
{
	auto pos = getEntityPos(entity);
	constexpr sf::Vector2f offset{ 40, 40 };
	return { pos + offset };
}

sf::Vector2i Utilities::getEntityCell(const Entity& entity)
{
	auto pos = getEntityVisualPosition(entity);
	return getCellIndex(pos);
}

sf::Vector2i Utilities::getEntityCellRaw(const Entity& entity)
{
	return getCellIndex(getEntityPos(entity));
}

bool Utilities::isEntityIdling(const Entity& entity)
{
	return entity.getComponent<EntityStateComponent>().cCurrentState == EntityState::Idle;
}

const sf::Color& Utilities::getVisibleTileColor(const Tile& tile)
{
	if (tile.tileType == TileType::Wall)
		return Config::wallTileColor;

	return Config::floorTileColor;
}

const sf::Color& Utilities::getExploredTileColor(const Tile& tile)
{
	if (tile.tileType == TileType::Wall)
		return Config::dimmedWallTileColor;

	return Config::dimmedFloorTileColor;
}

bool Utilities::isEntityWithinFOVRange(const Entity& observer, const Entity& target)
{
	
	auto observerCell = Utilities::getEntityCell(observer);
	auto targetCell = Utilities::getEntityCell(target);

	auto dx = std::abs(observerCell.x - targetCell.x);
	auto dy = std::abs(observerCell.y - targetCell.y);
	auto fovRange = observer.getComponent<FieldOfViewComponent>().cRangeOfView;

	return dx <= fovRange && dy <= fovRange;
}

bool Utilities::isEntityWithinAttackRange(const Entity& attacker, const Entity& target)
{
	auto& statsComp = attacker.getComponent<CombatStatsComponent>();
	auto range = statsComp.cAttackRange;

	auto attackerCell = Utilities::getEntityCell(attacker);
	auto targetCell = Utilities::getEntityCell(target);

	auto dx = std::abs(attackerCell.x - targetCell.x);
	auto dy = std::abs(attackerCell.y - targetCell.y);
	return (dx <= range && dy == 0) || (dy <= range && dx == 0);
}

Direction Utilities::getDirectionToTarget(const Entity& entity, const Entity& target)
{
	auto fromCell = getEntityCell(entity);
	auto toCell = getEntityCell(target);
	
	int dx = toCell.x - fromCell.x;
	int dy = toCell.y - fromCell.y;

	if (dx != 0)
		return dx > 0 ? Direction::Right : Direction::Left;
	else if (dy != 0)
		return dy > 0 ? Direction::Bottom : Direction::Up;

	return Direction::Up;
}

void Utilities::setEntityDirection(const Entity& entity, Direction dir)
{
	entity.getComponent<DirectionComponent>().cCurrentDir = dir;
}

sf::Vector2f Utilities::calculateNewBarSize(const Entity& entity, const sf::Vector2f& originalSize)
{
	const auto& statsComp = entity.getComponent<CombatStatsComponent>();
	if (statsComp.cHealth <= 0)
		return { 0.f, originalSize.y };

	float fCurrHp = static_cast<float>(statsComp.cHealth);
	float fMaxHp = static_cast<float>(statsComp.cMaxHealth);

	float hpRatio = fCurrHp / fMaxHp;
	float xSize = hpRatio * originalSize.x;

	return sf::Vector2f{ xSize, originalSize.y };
}

sf::Vector2f Utilities::calculateNewBarSize(const sf::Vector2f& originalSize, int minV, int maxV)
{
	float fMin = static_cast<float>(minV);
	float fMax = static_cast<float>(maxV);

	float ratio = fMin / fMax;
	float xSize = ratio * originalSize.x;
	if (xSize < 0)
		xSize = 0.f;
	return { xSize, originalSize.y };
}

void Utilities::changeHpBarSize(const Entity& entity, const sf::Vector2f& newSize)
{
	auto& hpBarComp = entity.getComponent<HealthBarComponent>();
	hpBarComp.cForegroundBar.setSize(newSize);
}

void Utilities::changeBarSize(sf::RectangleShape& rect, const sf::Vector2f& newSize)
{
	rect.setSize(newSize);
}

void Utilities::scaleSprite(sf::Sprite& sprite, const sf::Vector2f& newSize)
{
	auto spriteSize = sprite.getTexture().getSize();
	float fSpriteWidth = static_cast<float>(spriteSize.x);
	float fSpriteHeight = static_cast<float>(spriteSize.y);
	sf::Vector2f factor{ newSize.x / fSpriteWidth, newSize.y / fSpriteHeight };

	sprite.scale(factor);
}

sf::Color Utilities::lerpColor(const sf::Color& startColor, const sf::Color& endColor, float t)
{
	return sf::Color
	{
		static_cast<std::uint8_t>(startColor.r + (endColor.r - startColor.r) * t),
		static_cast<std::uint8_t>(startColor.g + (endColor.g - startColor.g) * t),
		static_cast<std::uint8_t>(startColor.b + (endColor.b - startColor.b) * t),
		startColor.a
	};
}

bool Utilities::isHealingSpell(SpellIdentifier spellid)
{
	constexpr std::array<SpellIdentifier, 2> healingSpells =
	{
		SpellIdentifier::QuickHeal, SpellIdentifier::MajorHeal
	};

	return std::ranges::find(healingSpells, spellid) != std::ranges::end(healingSpells);
}

bool Utilities::isSpellCastAnimation(AnimationIdentifier animId)
{
	return animId == AnimationIdentifier::GenericSpellCast ||
		   animId == AnimationIdentifier::GenericShoot;
}

bool Utilities::isAnAttackAnimation(AnimationIdentifier animId)
{
	return animId == AnimationIdentifier::Attack1 ||
		   animId == AnimationIdentifier::Attack2 ||
		   animId == AnimationIdentifier::Attack3;

}

sf::Vector2f Utilities::dirToVector(Direction dir)
{
	switch (dir)
	{
	case Direction::Up:
		return { 0.f, -1.f };
	case Direction::Bottom:
		return { 0.f, 1.f };
	case Direction::Left:
		return { -1.f, 0.f };
	case Direction::Right:
		return { 1.f, 0.f };
	default:
		return { 0.f, 0.f };
	}
	return { 0.f, 0.f };
}

void Utilities::setTextOriginOnCenter(sf::Text& text)
{
	auto size = text.getGlobalBounds().size;
	text.setOrigin(size * 0.5f);
}

std::string Utilities::wrapText(const std::string& originalStr, const sf::Font& font, float maxWidth, unsigned int charSize)
{
	sf::Text text(font, "", charSize);
	std::string wrappedText;

	auto splitStr = originalStr | std::views::split(' ');
	std::string fullLine = "";

	for (const auto& wordView : splitStr)
	{
		std::string word{ std::ranges::begin(wordView), std::ranges::end(wordView) };
		std::string potentialNewLine = "";

		if (fullLine.empty())
			potentialNewLine = word;
		else
			potentialNewLine = fullLine + " " + word;

		text.setString(potentialNewLine);
		auto textWidth = text.getLocalBounds().size.x;

		if (textWidth >= maxWidth)
		{
			if (!fullLine.empty())
				wrappedText += fullLine + "\n";
			fullLine = word;
		}
		else
		{
			if (!fullLine.empty())
				fullLine += " ";
			fullLine += word;
		}
	}
	if (!fullLine.empty())
		wrappedText += fullLine;

	return wrappedText;
}

bool Utilities::strToBool(std::string_view str)
{
	//for configuration files
	return str == "On";
}
