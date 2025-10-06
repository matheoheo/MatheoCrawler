#include "pch.h"
#include "GameOverviewSection.h"
#include "Utilities.h"
#include "Config.h"

GameOverviewSection::GameOverviewSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize)
	:IAboutSection(gameContext, sectionPos, sectionSize),
	mOverviewText(mFont),
	mControlsText(mFont, "Controls", mCharSize)
{
	createOverviewText();
	createControlsText();
	createTableData();
}

void GameOverviewSection::processEvents(sf::Event event)
{
}

void GameOverviewSection::update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime)
{
}

void GameOverviewSection::render()
{
	mGameContext.window.draw(mOverviewText);
	mGameContext.window.draw(mControlsText);
	renderTableLines();
	for (const auto& separator : mSeparators)
		mGameContext.window.draw(separator);
}

void GameOverviewSection::createOverviewText()
{
	const std::string introduceText = R"(Welcome to the MatheoCrawler!
Step into a dangerous dungeon filled with monsters.

Your goal is to explore each floor, defeat enemies and collect gold to grow stronger.

Each run is unique - rooms, enemies are randomly generated to keep the adventure fresh.

Choose your upgrades wisely and develop your character to survive deeper levels.)";

	std::string wrappedText = Utilities::wrapText(introduceText, mFont, getLeftColumnWidth(), mCharSize);

	mOverviewText.setCharacterSize(mCharSize);
	mOverviewText.setFillColor(mTextColor);
	mOverviewText.setString(wrappedText);
	mOverviewText.setPosition(mSectionPos);
}

void GameOverviewSection::createControlsText()
{
	mControlsText.setFillColor({ 220, 220, 230 });
	mControlsText.setOrigin(mControlsText.getGlobalBounds().size * 0.5f);
	const sf::Vector2f pos{
		getRightColumnPosX() + getRightColumnWidth() * 0.5f,
		mSectionPos.y
	};
	mControlsText.setPosition(pos);
}

void GameOverviewSection::createSeparators()
{
	constexpr float thinSize = 3.f;
	constexpr sf::Color sepColor{ 220, 210, 200, 150 };

	const float width = getRightColumnWidth() * 0.8f; //horizontal sep
	const float height = mTableLines.back().left.getPosition().y + mTableLines.back().left.getGlobalBounds().size.y - 
		mTableLines.front().left.getPosition().y; //vertical sep

	const sf::Vector2f vertPos{
		mControlsText.getPosition().x,
		getPosYBetweenLines(mTableLines.front(), mTableLines.back())
	};

	const sf::Vector2f horPos{
		mControlsText.getPosition().x,
		getPosYBetweenLines(mTableLines[0], mTableLines[2]) - thinSize * 0.5f
	};

	auto makeSep = [&sepColor](const sf::Vector2f& size, const sf::Vector2f& pos) -> sf::RectangleShape
	{
		sf::RectangleShape rect;
		rect.setFillColor(sepColor);
		rect.setSize(size);
		rect.setOrigin(size * 0.5f);
		rect.setPosition(pos);

		return rect;
	};

	mSeparators.push_back(makeSep({ width, thinSize }, horPos));
	mSeparators.push_back(makeSep({ thinSize, height }, vertPos));
}

void GameOverviewSection::createTableEntry(const std::string& leftStr, const std::string& rightStr)
{
	constexpr sf::Color entryColor{ sf::Color::Cyan };
	const unsigned int entryCharSize{ Config::getCharacterSize() / 2 };

	auto& entry = mTableLines.emplace_back(mFont);
	entry.left.setString(leftStr);
	entry.right.setString(rightStr);
	std::vector<sf::Text*> texts = { &entry.left, &entry.right };
	for (sf::Text* text : texts)
	{
		text->setFillColor(entryColor);
		text->setCharacterSize(entryCharSize);
		text->setOrigin(text->getGlobalBounds().size * 0.5f);
	}
}

void GameOverviewSection::createTableData()
{
	struct Entry {
		std::string_view left;
		std::string_view right;
	};
	constexpr size_t entryCount = 11;
	constexpr std::array<Entry, entryCount> entries =
	{ {
		{"Action",        "Key"},
		{"",              ""},
		{"Move Up",       "W"},
		{"Move Down",     "S"},
		{"Move Left",     "A"},
		{"Move Right",    "D"},
		{"Attack",        "Space"},
		{"Open Shop",     "M"},
		{"Use Spell",     "Z/X/C/V/B"},
		{"Choose Attack", "1/2/3"},
		{"Pause Menu",    "Escape"},
	} };

	for (const auto& entry : entries)
		createTableEntry(std::string(entry.left), std::string(entry.right));
	positionTableData();
	createSeparators();
}

void GameOverviewSection::positionTableData()
{
	const float margin = mCharSize * 0.35f;
	const float entryMargin = static_cast<float>(mTableLines.front().left.getCharacterSize());
	auto controlBounds = mControlsText.getGlobalBounds();
	float y = controlBounds.position.y + controlBounds.size.y + margin;

	for (auto& line : mTableLines)
	{
		line.left.setPosition({ getLeftTableX(), y });
		line.right.setPosition({ getRightTableX(), y });
		y += margin + entryMargin;
	}
}

void GameOverviewSection::renderTableLines()
{
	for (const auto& line : mTableLines)
	{
		mGameContext.window.draw(line.left);
		mGameContext.window.draw(line.right);
	}
}

float GameOverviewSection::getLeftTableX() const
{
	constexpr float margin = 0.25f;
	return getRightColumnPosX() + getRightColumnWidth() * margin;
}

float GameOverviewSection::getRightTableX() const
{
	constexpr float margin = 0.75f;
	return getRightColumnPosX() + getRightColumnWidth() * margin;
}

float GameOverviewSection::getPosYBetweenLines(const TableLine& l1, const TableLine& l2) const
{
	auto pos1 = l1.left.getPosition().y;
	auto pos2 = l2.left.getPosition().y;

	auto posDiff = pos2 - pos1;
	return pos1 + posDiff * 0.5f;
}

float GameOverviewSection::getLeftColumnWidth() const
{
	constexpr float widthPercent = 0.6f;
	return mSectionSize.x * widthPercent;
}

float GameOverviewSection::getRightColumnPosX() const
{
	return mSectionPos.x + getLeftColumnWidth() + 1.f;
}

float GameOverviewSection::getRightColumnWidth() const
{
	float x = getRightColumnPosX();
	float fullSecWidth = mSectionPos.x + mSectionSize.x;
	return fullSecWidth - x;
}

