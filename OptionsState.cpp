#include "pch.h"
#include "OptionsState.h"
#include "Config.h"
#include "Utilities.h"

OptionsState::OptionsState(GameContext& gameContext)
	:IState(gameContext),
	mCharSize(Config::getCharacterSize()),
	mMarginPercent(0.075f),
	mBackButton(gameContext.fonts.get(FontIdentifiers::Default), "Go Back", mCharSize),
	mBackground(gameContext.textures.get(TextureIdentifier::OptionsBackground)),
	mHeaderText(gameContext.fonts.get(FontIdentifiers::Default)),
	mApplyChangesText(gameContext.fonts.get(FontIdentifiers::Default))
{
}

void OptionsState::onEnter()
{
	createBackButton();
	createBackground();
	createHeaderText();
	createApplyChangesText();
	createOptionIdToStringMap();
	createOptions();
}

void OptionsState::processEvents(sf::Event event)
{
	if (mBackButton.isPressed(event))
	{
		mBackButton.invoke();
		return;
	}

	for (auto& [key, val] : mOptions)
		val.processClick(event);
}

void OptionsState::update(const sf::Time& deltaTime)
{
	IState::updateMousePosition();
	mBackButton.update(fMousePos);

	for (auto& [key, val] : mOptions)
		val.update(fMousePos);
}

void OptionsState::render()
{
	mGameContext.window.draw(mBackground);
	mGameContext.window.draw(mHeaderText);
	mGameContext.window.draw(mApplyChangesText);
	mBackButton.render(mGameContext.window);

	for (auto& [key, val] : mOptions)
		val.render(mGameContext.window);
}

void OptionsState::createBackButton()
{
	mBackButton.setCallback([this]() {
		mGameContext.eventManager.notify<PopStateEvent>(PopStateEvent());
		});

	const sf::Vector2f pos{
		Config::fWindowSize.x * mMarginPercent, //so we get left position
		Config::fWindowSize.y * (1.0f - mMarginPercent) //so we got bottom position
	};
	mBackButton.setPosition(pos);
}

void OptionsState::createBackground()
{
	Utilities::scaleSprite(mBackground, Config::fWindowSize);
}

void OptionsState::createHeaderText()
{
	mHeaderText.setFillColor({ 255, 255, 100 });
	mHeaderText.setString("Crawler's Options");
	mHeaderText.setCharacterSize(mCharSize * 2);
	auto textSize = mHeaderText.getGlobalBounds().size;
	mHeaderText.setOrigin(textSize * 0.5f);
	
	constexpr float marginRatio = 0.05f;
	const sf::Vector2f pos{
		Config::fWindowSize.x * 0.5f,
		Config::fWindowSize.y * marginRatio
	};

	mHeaderText.setPosition(pos);
	mHeaderText.setOutlineThickness(3.f);
	mHeaderText.setOutlineColor({ 10, 20, 50 });
	mHeaderText.setStyle(sf::Text::Style::Bold);
}

void OptionsState::createApplyChangesText()
{
	mApplyChangesText.setFillColor(sf::Color::White);
	mApplyChangesText.setString("Please restart the game to apply changes.");
	mApplyChangesText.setCharacterSize(mCharSize);

	mApplyChangesText.setOutlineThickness(2.f);
	mApplyChangesText.setOutlineColor(sf::Color::Black);
	mApplyChangesText.setStyle(sf::Text::Style::Italic);
	mApplyChangesText.setOrigin(mApplyChangesText.getGlobalBounds().size * 0.5f);

	constexpr float bottomMargin = 0.25f;
	const sf::Vector2f pos{
		Config::fWindowSize.x * 0.5f,
		Config::fWindowSize.y * (1.0f - bottomMargin)
	};
	mApplyChangesText.setPosition(pos);
}

std::string OptionsState::optionIDToString(OptionID id) const
{
	if (mOptionIdToStringMap.contains(id))
		return mOptionIdToStringMap.at(id);

	return "";
}

void OptionsState::createOptionIdToStringMap()
{
	mOptionIdToStringMap[OptionID::AntiAliasing] = "AntiAlias";
	mOptionIdToStringMap[OptionID::Fullscreen]   = "Fullscreen";
	mOptionIdToStringMap[OptionID::Resolution]   = "Resolution";
	mOptionIdToStringMap[OptionID::VSync]        = "VSync";
}

void OptionsState::createOptions()
{
	const auto& font = mGameContext.fonts.get(FontIdentifiers::Default);
	for (const auto& [key, val] : mOptionIdToStringMap)
	{
		//vector::emplace, by default, creates object in place,
		//sadly, unordered_map::emplace doesn't work like that
		//this is the way, to create an object in place:
		mOptions.emplace(
			std::piecewise_construct,
			std::forward_as_tuple(key),
			std::forward_as_tuple(font, val +":")
		);
	}
	createResolutionOptionValues();
	createAntiAliasingOptionValues();
	createBooleanValues(mOptions.at(OptionID::Fullscreen));
	createBooleanValues(mOptions.at(OptionID::VSync));
	positionOptions();
}

void OptionsState::createResolutionOptionValues()
{
	auto& option = mOptions.at(OptionID::Resolution);
	auto modes = sf::VideoMode::getFullscreenModes();
	auto bestBpp = modes[0].bitsPerPixel; //they come sorted, so we can take bestBpp right like this.

	auto filteredModes = modes | 
		std::ranges::views::filter([bestBpp](const auto& mode)
		{
			return mode.bitsPerPixel == bestBpp;
		});

	for (const auto& mode : filteredModes)
		option.addValue(getResolutionValue(mode.size));
}

void OptionsState::createAntiAliasingOptionValues()
{
	auto& option = mOptions.at(OptionID::AntiAliasing);
	constexpr size_t valuesCount = 4;
	constexpr std::array<std::string_view, valuesCount> aaValues =
	{
		"0x", "2x", "4x", "8x"
	};

	for (const auto& val : aaValues)
		option.addValue(std::string(val));
}

void OptionsState::createBooleanValues(OptionSelector& option)
{
	option.addValue("On");
	option.addValue("Off");
}

std::string OptionsState::getResolutionValue(const sf::Vector2u& res) const
{
	return std::to_string(res.x) + "x" + std::to_string(res.y);
}

void OptionsState::positionOptions()
{

	//we must find width of the longest label of our options.
	//This will allow us to properly position all values in the same position of X axis.
	auto labelIt = std::ranges::max_element(mOptions | std::views::values, {}, &OptionSelector::getLabelWidth);
	const float labelWidth = (*labelIt).getLabelWidth();

	//We will also need longest [value] from our mOptions.
	auto valueIt = std::ranges::max_element(mOptions | std::views::values, {}, &OptionSelector::getLongestValueWidth);
	const float valueWidth = (*valueIt).getLongestValueWidth();

	//some margins, based on visualization
	constexpr float marginInRow = 12.f;
	const float marginBetweenOptions = mCharSize * 0.25f;

	const float labelX = Config::fWindowSize.x * mMarginPercent;
	const float headerHeight = mHeaderText.getGlobalBounds().size.y;
	float y = mHeaderText.getPosition().y + headerHeight + (headerHeight * mMarginPercent);

	const float valueX = labelX + labelWidth + marginInRow;
	const float arrowX = valueX + valueWidth + marginInRow;

	//we want to preverse proper ordering of options
	constexpr std::array<OptionID, 4> idsInOrder =
	{
		OptionID::Resolution, OptionID::Fullscreen, OptionID::AntiAliasing, OptionID::VSync
	};

	for (OptionID id : idsInOrder)
	{
		auto& opt = mOptions.at(id);
		opt.setPositions({ labelX, y }, { valueX, y }, { arrowX, y });
		y += mCharSize + marginBetweenOptions;
	}
}
