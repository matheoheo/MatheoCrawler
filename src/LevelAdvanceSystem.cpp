#include "pch.h"
#include "LevelAdvanceSystem.h"
#include "TileMap.h"
#include "Utilities.h"
#include "Config.h"
#include "MessageTypes.h"

LevelAdvanceSystem::LevelAdvanceSystem(SystemContext& systemContext, const TileMap& tileMap, const sf::Font& font)
	:ISystem(systemContext),
	mTileMap(tileMap),
	mAdvanceKey(sf::Keyboard::Key::E),
	mAdvanceText(font),
	mAdvanceTimer(0)
{
	registerToEvents();
}

void LevelAdvanceSystem::processEvents(const sf::Event event)
{
	if (isAdvanceKeyPressed(event) && canPlayerAdvance())
		notifyAdvanceEvent();
}

void LevelAdvanceSystem::update(const sf::Time& deltaTime)
{
	updateAdvanceText(deltaTime);
}

void LevelAdvanceSystem::render(sf::RenderWindow& window)
{
	if (isAdvanceTileVisible())
		window.draw(mAdvanceText);
}

void LevelAdvanceSystem::registerToEvents()
{
	registerToSetLevelAdvanceCellEvent();
	registerToPlayerMovedEvent();
}

void LevelAdvanceSystem::registerToSetLevelAdvanceCellEvent()
{
	mSystemContext.eventManager.registerEvent<SetLevelAdvanceCellEvent>([this](const SetLevelAdvanceCellEvent& data)
		{
			mLevelAdvanceCell = data.cell;
			createAdvanceText();
		});
}

void LevelAdvanceSystem::registerToPlayerMovedEvent()
{
	mSystemContext.eventManager.registerEvent<PlayerMoveFinishedEvent>([this](const PlayerMoveFinishedEvent& data)
		{
			auto playerTile = mTileMap.getTile(data.newPlayerPos);
			auto advanceTile = getAdvanceTile();
			if (!playerTile || !advanceTile)
				return;

			if (playerTile == advanceTile)
				sendAdvanceMessage();
		});
}

const Tile* LevelAdvanceSystem::getAdvanceTile() const
{
	return mTileMap.getTile(mLevelAdvanceCell.x, mLevelAdvanceCell.y);
}

bool LevelAdvanceSystem::isAdvanceTileVisible() const
{
	const Tile* tile = getAdvanceTile();
	if (!tile)
		return false;

	return tile->visible;
}

bool LevelAdvanceSystem::isPlayerOnAdvanceCell() const
{
	const Tile* tile = getAdvanceTile();
	if (!tile)
		return false;

	return std::ranges::any_of(tile->occupyingEntities, [](const Entity* ent)
		{
			return ent->hasComponent<PlayerComponent>();
		}); 
}

bool LevelAdvanceSystem::canPlayerAdvance() const
{
	const auto& player = mSystemContext.entityManager.getPlayer();
	return isPlayerOnAdvanceCell() && Utilities::isEntityIdling(player);
}

bool LevelAdvanceSystem::isAdvanceKeyPressed(const sf::Event event)
{
	if (const auto* data = event.getIf<sf::Event::KeyPressed>())
		return data->code == mAdvanceKey;

	return false;
}

void LevelAdvanceSystem::notifyAdvanceEvent()
{
	mSystemContext.eventManager.notify<LoadNextLevelEvent>(LoadNextLevelEvent());
}

void LevelAdvanceSystem::createAdvanceText()
{
	mAdvanceText.setFillColor({ 185, 150, 0 });
	mAdvanceText.setCharacterSize(Config::getCharacterSize() / 3);
	mAdvanceText.setString("ADVANCE");
	mAdvanceText.setStyle(sf::Text::Style::Bold);
	mAdvanceText.setOrigin({ mAdvanceText.getGlobalBounds().size.x * 0.5f, mAdvanceText.getGlobalBounds().size.y });
	positionTextOnCellBottom();
}

void LevelAdvanceSystem::positionTextOnCellBottom()
{
	const float halfCell = Config::getCellSize().x * 0.5f;
	const sf::Vector2f pos{ mLevelAdvanceCell.x * Config::getCellSize().x + halfCell,
		mLevelAdvanceCell.y * Config::getCellSize().y + Config::getCellSize().y };
	
	mAdvanceText.setPosition(pos);
}

void LevelAdvanceSystem::updateAdvanceText(const sf::Time& deltaTime)
{
	if (!isAdvanceTileVisible())
		return;

	constexpr float maxAdvanceTime = 1000.f; //in ms;
	constexpr float moveSpeed = 64.f;
	constexpr sf::Vector2f dir{ 0, -1.f };
	constexpr std::uint8_t maxAlpha = 255;

	mAdvanceTimer += deltaTime.asMilliseconds();
	const float progress = std::clamp(1.f - (mAdvanceTimer / maxAdvanceTime), 0.f, 1.0f);

	auto color = mAdvanceText.getFillColor();
	color.a = static_cast<std::uint8_t>(progress * maxAlpha);
	mAdvanceText.setFillColor(color);
	mAdvanceText.move(dir * moveSpeed * deltaTime.asSeconds());
	
	if (mAdvanceTimer > maxAdvanceTime)
	{
		color.a = maxAlpha;
		mAdvanceText.setFillColor(color);
		positionTextOnCellBottom();
		mAdvanceTimer = 0;
	}
}

void LevelAdvanceSystem::sendAdvanceMessage()
{
	mSystemContext.eventManager.notify<LogMessageEvent>(LogMessageEvent(MessageType::CanAdvance));
}
