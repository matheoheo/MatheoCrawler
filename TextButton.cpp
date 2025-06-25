#include "pch.h"
#include "TextButton.h"
#include "Config.h"

TextButton::TextButton(const sf::Font& font, const std::string& label, const unsigned int charSize, const sf::Vector2f& position)
	:mText(font, label, charSize),
	mCallback([]() {}),
	mDefaultColor(sf::Color::White),
	mHoverColor(sf::Color{255, 200, 0}),
	mInactiveColor(80, 80, 80)
{
	setPosition(position);
}

void TextButton::setCallback(ButtonCallback callback)
{
	mCallback = std::move(callback);
}

void TextButton::setPosition(const sf::Vector2f& position)
{
	mText.setPosition(position);
}

void TextButton::setCharacterSize(unsigned int size)
{
	mText.setCharacterSize(size);
}

void TextButton::setDefaultColor(const sf::Color& color)
{
	mDefaultColor = color;
}

void TextButton::setHoverColor(const sf::Color& color)
{
	mHoverColor = color;
}

void TextButton::setOriginOnCenter()
{
	auto bounds = mText.getGlobalBounds();
	mText.setOrigin(bounds.size * 0.5f);
}

void TextButton::setActiveState(bool state)
{
	mActive = state;
}

sf::Vector2f TextButton::getSize() const
{
	return mText.getGlobalBounds().size;
}

sf::Vector2f TextButton::getPosition() const
{
	return mText.getPosition();
}

const sf::Color& TextButton::getDefaultColor() const
{
	return mDefaultColor;
}

std::string TextButton::getButtonString() const
{
	return mText.getString();
}

bool TextButton::isPressed(sf::Event event)
{
	if (!mActive)
		return false;

	if (const auto pressedEvent = event.getIf<sf::Event::MouseButtonPressed>())
	{
		if (pressedEvent->button == sf::Mouse::Button::Left)
		{
			sf::Vector2f pos{ static_cast<float>(pressedEvent->position.x), static_cast<float>(pressedEvent->position.y) };
			return mText.getGlobalBounds().contains(pos);
		}
	}
	return false;
}

void TextButton::invoke()
{
	mCallback();
}

void TextButton::update(const sf::Vector2f& fMousePos)
{
	if (!mActive)
		mText.setFillColor(mInactiveColor);
	else
		mText.setFillColor(mText.getGlobalBounds().contains(fMousePos) ? mHoverColor : mDefaultColor);
}

void TextButton::render(sf::RenderWindow& window) const
{
	window.draw(mText);
}