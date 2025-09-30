#include "pch.h"
#include "OptionsState.h"
#include "Config.h"
#include "Utilities.h"

OptionsState::OptionsState(GameContext& gameContext)
	:IState(gameContext),
	mCharSize(Config::getCharacterSize()),
	mLeftMarginPercent(0.075f),
	mRightMarginPercent(0.85f),
	mBackground(gameContext.textures.get(TextureIdentifier::OptionsBackground)),
	mHeaderText(gameContext.fonts.get(FontIdentifiers::Default)),
	mApplyChangesText(gameContext.fonts.get(FontIdentifiers::Default)),
	mConfigSaved(false)
{
}

void OptionsState::onEnter()
{
	createButtons();
	createBackground();
	createHeaderText();
	createApplyChangesText();
	createOptionIdToStringMap();
	createOptions();
}

void OptionsState::processEvents(sf::Event event)
{
	for (auto& btn : mButtons)
	{
		if (btn.isPressed(event))
		{
			btn.invoke();
			return;
		}
	}

	for (auto& [key, val] : mOptions)
		val.processClick(event);
}

void OptionsState::update(const sf::Time& deltaTime)
{
	IState::updateMousePosition();
	for (auto& btn : mButtons)
		btn.update(fMousePos);

	for (auto& [key, val] : mOptions)
		val.update(fMousePos);
}

void OptionsState::render()
{
	mGameContext.window.draw(mBackground);
	mGameContext.window.draw(mHeaderText);

	if(mConfigSaved)
		mGameContext.window.draw(mApplyChangesText);

	for (const auto& btn : mButtons)
		btn.render(mGameContext.window);

	for (auto& [key, val] : mOptions)
		val.render(mGameContext.window);
}

void OptionsState::createButtons()
{
	constexpr size_t buttonsCount = 2;
	const auto& font = mGameContext.fonts.get(FontIdentifiers::Default);
	const std::array<TextButton::TextButtonInitData, 2> buttonsData =
	{ {
		{.label = "Apply Changes", .callback = [this]() {
				applyChanges();
			}
		},
		{.label = "Go Back", .callback = [this]() {
				mGameContext.eventManager.notify<PopStateEvent>(PopStateEvent());
			}
		}
	} };
	const std::array<sf::Vector2f, buttonsCount> buttonsPos =
	{
		Utilities::getPrimaryButtonPos(),
		Utilities::getSecondaryButtonsPos()
	};

	mButtons.reserve(buttonsCount);

	for (int i = 0; i < buttonsCount; ++i)
	{
		mButtons.emplace_back(font, buttonsData[i].label, mCharSize);
		mButtons.back().setCallback(buttonsData[i].callback);
		mButtons.back().setPosition(buttonsPos[i]);
	}
}

void OptionsState::createBackground()
{
	Utilities::scaleSprite(mBackground, Config::fWindowSize);
}

void OptionsState::createHeaderText()
{
	mHeaderText = Utilities::createStateHeaderText(mGameContext.fonts.get(FontIdentifiers::Default), "Crawler's Options");
	return;

	mHeaderText.setFillColor({ 255, 255, 100 });
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
	mApplyChangesText.setString("Configuration has been saved. Please restart the game to apply changes.");
	mApplyChangesText.setCharacterSize(mCharSize);

	mApplyChangesText.setOutlineThickness(2.f);
	mApplyChangesText.setOutlineColor(sf::Color::Black);
	mApplyChangesText.setOrigin(mApplyChangesText.getGlobalBounds().size * 0.5f);

	constexpr float bottomMargin = 0.25f;
	const sf::Vector2f pos{
		Config::fWindowSize.x * 0.5f,
		Config::fWindowSize.y * (1.0f - bottomMargin)
	};
	mApplyChangesText.setPosition(pos);
}

void OptionsState::applyChanges()
{
	auto resolution = getResolutionValue();
	auto antiAliasingLevel = getAntiAliasingValue();
	bool fullscreen = Utilities::strToBool(mOptions.at(OptionID::Fullscreen).getValue());
	bool vsync = Utilities::strToBool(mOptions.at(OptionID::VSync).getValue());
	Config::saveConfigToFile(resolution, antiAliasingLevel, fullscreen, vsync);

	mConfigSaved = true;
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
			std::forward_as_tuple(font, val + ":")
		);
	}
	createResolutionOptionValues();
	createAntiAliasingOptionValues();
	createBooleanValues(mOptions.at(OptionID::Fullscreen));
	createBooleanValues(mOptions.at(OptionID::VSync));
	positionOptions();
	setActualValuesToOptions();
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
		option.addValue(resolutionToString(mode.size));

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

	const float labelX = Config::fWindowSize.x * (5*mLeftMarginPercent);
	const float headerHeight = mHeaderText.getGlobalBounds().size.y;
	float y = mHeaderText.getPosition().y + headerHeight + (headerHeight );

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

void OptionsState::setActualValuesToOptions()
{
	const std::array<std::pair<OptionID, std::string>, 4> values =
	{ {
		{OptionID::AntiAliasing, std::to_string(Config::aaLevel) + "x"},
		{OptionID::Fullscreen,   Utilities::boolToStr(Config::fullscreen)},
		{OptionID::VSync,		 Utilities::boolToStr(Config::vSync)},
		{OptionID::Resolution,   resolutionToString(Config::windowSize)}
	} };

	for (auto& [key, val] : mOptions)
	{
		auto it = std::ranges::find_if(values, [key](const auto& pair) {
			return pair.first == key;
		});

		if (it != std::ranges::end(values))
			val.setValue(it->second);
	}
}

std::string OptionsState::resolutionToString(const sf::Vector2u& res) const
{
	return std::to_string(res.x) + "x" + std::to_string(res.y);
}

sf::Vector2u OptionsState::getResolutionValue() const
{
	//need to take option by option, parse and save to file
	auto resolutionValue = mOptions.at(OptionID::Resolution).getValue();
	//resolution comes in 1920x1080 way, so we must split it
	auto splitPos = resolutionValue.find('x');
	std::string width = resolutionValue.substr(0, splitPos);
	std::string height = resolutionValue.substr(splitPos + 1, resolutionValue.size() - 1);

	return
	{
		static_cast<unsigned int>(std::stoi(width)),
		static_cast<unsigned int>(std::stoi(height))
	};
}

unsigned int OptionsState::getAntiAliasingValue() const
{
	std::string value = mOptions.at(OptionID::AntiAliasing).getValue();
	//AntiAliasing comes in a format '0x', '2x', etc.. so we just need to get first char and thats all
	std::string opt{ value[0] };

	return static_cast<unsigned int>(std::stoi(opt));
}
