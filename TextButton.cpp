#include "pch.h"
#include "TextButton.h"
#include "Config.h"

TextButton::TextButton(const sf::Font& font, const std::string& label, const unsigned int charSize, const sf::Vector2f& position)
	:mText(font, label, charSize),
	mCallback([]() {})
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

bool TextButton::isPressed(sf::Event event)
{
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
	mText.setFillColor(mText.getGlobalBounds().contains(fMousePos) ? sf::Color{ 255, 165, 0 } : sf::Color::White);
}

void TextButton::render(sf::RenderWindow& window) const
{
	window.draw(mText);
}