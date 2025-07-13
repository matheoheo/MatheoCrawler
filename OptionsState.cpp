#include "pch.h"
#include "OptionsState.h"
#include "Config.h"
#include "Utilities.h"

OptionsState::OptionsState(GameContext& gameContext)
	:IState(gameContext),
	mCharSize(Config::getCharacterSize()),
	mBackButton(gameContext.fonts.get(FontIdentifiers::Default), "Go Back", mCharSize),
	mBackground(gameContext.textures.get(TextureIdentifier::OptionsBackground)),
	mOptionsText(gameContext.fonts.get(FontIdentifiers::Default))
{
}

void OptionsState::onEnter()
{
	createBackButton();
	createBackground();
	createOptionsText();
}

void OptionsState::processEvents(sf::Event event)
{
	if (mBackButton.isPressed(event))
	{
		mBackButton.invoke();
		return;
	}
}

void OptionsState::update(const sf::Time& deltaTime)
{
	IState::updateMousePosition();
	mBackButton.update(fMousePos);
}

void OptionsState::render()
{
	mGameContext.window.draw(mBackground);
	mGameContext.window.draw(mOptionsText);
	mBackButton.render(mGameContext.window);
}

void OptionsState::createBackButton()
{
	mBackButton.setCallback([this]() {
		mGameContext.eventManager.notify<PopStateEvent>(PopStateEvent());
		});

	constexpr float marginPercent = 0.075f;

	const sf::Vector2f pos{
		Config::fWindowSize.x * marginPercent, //so we get left position
		Config::fWindowSize.y * (1.0f - marginPercent) //so we got bottom position
	};
	mBackButton.setPosition(pos);
}

void OptionsState::createBackground()
{
	Utilities::scaleSprite(mBackground, Config::fWindowSize);
}

void OptionsState::createOptionsText()
{
	mOptionsText.setFillColor({ 255, 255, 100 });
	mOptionsText.setString("Crawler's Options");
	mOptionsText.setCharacterSize(mCharSize * 2);
	auto textSize = mOptionsText.getGlobalBounds().size;
	mOptionsText.setOrigin(textSize * 0.5f);
	
	constexpr float marginRatio = 0.05f;
	const sf::Vector2f pos{
		Config::fWindowSize.x * 0.5f,
		Config::fWindowSize.y * marginRatio
	};

	mOptionsText.setPosition(pos);
	mOptionsText.setOutlineThickness(3.f);
	mOptionsText.setOutlineColor({ 10, 20, 50 });
	mOptionsText.setStyle(sf::Text::Style::Bold);
}
