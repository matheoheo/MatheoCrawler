#pragma once
#include <functional>
class TextButton
{
public:
	using ButtonCallback = std::function<void()>;

	//Data structure for initalizing a TextButton object
	//This struct is used outside of this class to setup the button parametrs before creating a TextButton instance.
	struct TextButtonInitData {
		std::string label;
		TextButton::ButtonCallback callback;
	};

	TextButton(const sf::Font& font, const std::string& label, const unsigned int charSize = 30, const sf::Vector2f& position = sf::Vector2f(0.f, 0.f));
	void setCallback(ButtonCallback callback);
	void setPosition(const sf::Vector2f& position);
	bool isPressed(sf::Event event);
	void invoke();

	//higlights the text if hovered over.
	void update(const sf::Vector2f& fMousePos);
	void render(sf::RenderWindow& window) const;
private:
	sf::Text mText;
	ButtonCallback mCallback;
};

