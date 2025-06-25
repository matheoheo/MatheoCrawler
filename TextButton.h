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
	void setCharacterSize(unsigned int size);
	void setDefaultColor(const sf::Color& color);
	void setHoverColor(const sf::Color& color);
	void setOriginOnCenter();
	void setActiveState(bool state);
	
	sf::Vector2f getSize() const;
	sf::Vector2f getPosition() const;
	const sf::Color& getDefaultColor() const;
	std::string getButtonString() const;

	bool isPressed(sf::Event event);
	void invoke();

	//higlights the text if hovered over.
	void update(const sf::Vector2f& fMousePos);
	void render(sf::RenderWindow& window) const;
private:
	sf::Text mText;
	ButtonCallback mCallback;
	sf::Color mDefaultColor;
	sf::Color mHoverColor;
	sf::Color mInactiveColor;
	bool mActive;
};

