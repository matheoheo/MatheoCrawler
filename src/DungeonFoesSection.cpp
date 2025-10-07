#include "pch.h"
#include "DungeonFoesSection.h"
#include "Config.h"
#include "Utilities.h"

DungeonFoesSection::DungeonFoesSection(GameContext& gameContext, const sf::Vector2f& sectionPos, const sf::Vector2f& sectionSize)
	:IAboutSection(gameContext, sectionPos, sectionSize),
	mSummary(mFont, "", mCharSize),
	mLastClicked(nullptr)
{
	mSummary.setFillColor(mTextColor);
	constexpr float scale = 0.075f;
	mIconSize = {
		Config::fWindowSize.y * scale,
		Config::fWindowSize.y * scale
	};
	yMargin = mIconSize.y * 0.2f;
	createEntries();
	createHighlighter();
	determineSummaryPos();
}

void DungeonFoesSection::processEvents(sf::Event event)
{
	if (auto data = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (data->button != sf::Mouse::Button::Left)
			return;

		const sf::Vector2f fPos{
			static_cast<float>(data->position.x),
			static_cast<float>(data->position.y)
		};
		
		for (auto& entry : mFoeEntries)
		{
			if (entry.bounds.contains(fPos))
			{
				mLastClicked = &entry;
				return;
			}
		}
	}
}

void DungeonFoesSection::update(const sf::Vector2f& fMousePos, const sf::Time& deltaTime)
{
	updateHighlighter(fMousePos);
	updateOnClick();
}

void DungeonFoesSection::render()
{
	for (const auto& entry : mFoeEntries)
	{
		mGameContext.window.draw(entry.sprite);
		mGameContext.window.draw(entry.name);
	}

	mGameContext.window.draw(mHighlighter);
	mGameContext.window.draw(mSelected);
	mGameContext.window.draw(mSummary);
}

void DungeonFoesSection::createFoeEntry(const sf::Texture& texture, const std::string& nameStr, const std::string& summaryStr)
{
	constexpr sf::Color nameColor{ 230, 210, 140 };
	constexpr sf::IntRect textureRect(sf::Vector2i(0, 128), sf::Vector2i(64, 64));
	
	auto& entry = mFoeEntries.emplace_back(texture, mFont, mCharSize);
	entry.sprite.setTextureRect(textureRect);
	auto bounds = entry.sprite.getLocalBounds();
	entry.sprite.setScale({
		mIconSize.x / bounds.size.x,
		mIconSize.y / bounds.size.y
		});
	entry.name.setString(nameStr);
	entry.name.setFillColor(nameColor);
	auto nameBounds = entry.name.getGlobalBounds();
	entry.name.setOrigin({ 0.f, nameBounds.size.y * 0.5f });
	entry.summaryStr = summaryStr;
}

void DungeonFoesSection::createEntries()
{
	struct EntryData {
		TextureIdentifier id;
		std::string_view name;
		std::string_view lore;
	};
	constexpr std::size_t count = 4;
	mFoeEntries.reserve(count);
	constexpr std::array<EntryData, count> entriesData =
	{ {
		{.id = TextureIdentifier::Skletorus, .name = "Skletorus", .lore = R"(A restless skeleton that wields a chipped axe with grim precision.

Once a harmless soldier buried in catacombs, Skletorus rose when the dungeon began to stir. His bones remember battle even if his mind does not. Each swing of his axes echoes the discipline of life long forgotten.)"},
		{.id = TextureIdentifier::Bonvik,    .name = "Bonvik",    .lore = R"(A horned beast with wings of lizard and fury of the storm.

Bonvik's creation is mystery whispered among cultists - a failed attemp to fuse beast and demon. The wings may not lift him far, but his charge can crush stone. His roar alone is enough to shake torches from the walls.)"},
		{.id = TextureIdentifier::Moranna,   .name = "Moranna",   .lore = R"(A water mage whose calm hides the weight of the tides.

Moranna channels the dungeon's ancient reservoirs, bending water to her will. She speaks softly as she attacks, as if mourning each ripple she must unleash. Those struck by her spells often feel as tough as the ocean itself has turned against them.)"},
		{.id = TextureIdentifier::Ray,       .name = "Ray",       .lore = R"(A copper-forged construct pulsing with arcane energy.

Ray was once a royal guardian, its metal body forget to protect forbidden knowledge. When the halls fell silent, he remained - hollow, patient, and unyielding. His copper shell now hums with the echoes of every spell he has absorbed.)"}
	} };

	for (const auto& entry : entriesData)
	{
		const auto& txt = mGameContext.textures.get(entry.id);
		createFoeEntry(txt, std::string(entry.name), std::string(entry.lore));
	}
	positionEntries();
	setEntiresBounds();
}

void DungeonFoesSection::positionEntries()
{
	const float yStep = mIconSize.y + yMargin;
	sf::Vector2f pos = mSectionPos;
	
	for (auto& entry : mFoeEntries)
	{
		entry.sprite.setPosition(pos);
		sf::Vector2f namePos{
			pos.x + mIconSize.x + yMargin,
			pos.y + mIconSize.y * 0.45f
		};
		entry.name.setPosition(namePos);
		pos.y += yStep;
	}
}

void DungeonFoesSection::setEntiresBounds()
{
	//We need to find longest bounds to determine an uniform size.
	auto longestIt = std::ranges::max_element(mFoeEntries, [](const FoeEntry& e1, const FoeEntry& e2)
		{
			return e1.name.getGlobalBounds().size.x < e2.name.getGlobalBounds().size.x;
		});

	auto& longestEntry = *longestIt;
	float halfMargin = yMargin * 0.5f;
	float boundsWidth = (longestEntry.name.getPosition().x + longestEntry.name.getGlobalBounds().size.x) - mSectionPos.x + yMargin;
	float boundsHeight = mIconSize.y + yMargin;

	for (auto& entry : mFoeEntries)
	{
		entry.bounds.size = { boundsWidth, boundsHeight };
		entry.bounds.position = sf::Vector2f{
			entry.sprite.getPosition().x - halfMargin,
			entry.sprite.getPosition().y - halfMargin
		};
		std::cout << std::endl;
	}
}

void DungeonFoesSection::createHighlighter()
{
	const auto& entry = mFoeEntries.front();

	mHighlighter.setSize(entry.bounds.size);
	mHighlighter.setFillColor(sf::Color::Transparent);
	mHighlighter.setOutlineThickness(1.5f);
	mHighlighter.setOutlineColor({255, 200, 150, 160});
	mHighlighter.setPosition({ -1000.f, -1000.f }); //out of screen for start

	mSelected = mHighlighter;
	mSelected.setOutlineColor({ 255, 165, 0, 230 });
}

void DungeonFoesSection::updateHighlighter(const sf::Vector2f& mousePos)
{
	for (const auto& entry : mFoeEntries)
	{
		if (entry.bounds.contains(mousePos))
		{
			mHighlighter.setPosition(entry.bounds.position);
			return;
		}
	}
	mHighlighter.setPosition({ -1000.f, -1000.f });
}

void DungeonFoesSection::updateOnClick()
{
	if (!mLastClicked)
		return;
	mSelected.setPosition(mLastClicked->bounds.position);
	createSummaryOnClick();

	mLastClicked = nullptr;
}

void DungeonFoesSection::determineSummaryPos()
{
	const auto& entry = mFoeEntries.front();
	const float margin = Config::fWindowSize.x * 0.08f;
	mSummaryPos = sf::Vector2f{
		entry.bounds.position.x + entry.bounds.size.x + margin,
		entry.name.getPosition().y - entry.name.getOrigin().y
	};

}

void DungeonFoesSection::createSummaryOnClick()
{
	float width = mSectionSize.x - (mSummaryPos.x - mSectionPos.x);
	std::string wrappedTxt = Utilities::wrapText(mLastClicked->summaryStr, mFont, width, mCharSize);
	mSummary.setPosition(mSummaryPos);
	mSummary.setString(wrappedTxt);
}
