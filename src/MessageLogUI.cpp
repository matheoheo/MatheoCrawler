#include "pch.h"
#include "MessageLogUI.h"
#include "Config.h"
MessageLogUI::MessageLogUI(GameContext& gameContext, Entity& player)
	:UIComponent(gameContext, player),
	mFont(gameContext.fonts.get(FontIdentifiers::Default)),
	mMaxMessages(5),
	mLifetimeLimit(3000),
	mCharSize(Config::getCharacterSize() / 2),
	mMargin(10.f),
	mLogPosition(mMargin, Config::fWindowSize.y - mCharSize - mMargin)
{
	createMessageColors();
	registerToEvents();
}

void MessageLogUI::processEvents(const sf::Event event)
{
}

void MessageLogUI::update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime)
{
	addTimeToMessages(deltaTime);
}

void MessageLogUI::render()
{
	for (const auto& message : mLogMessages)
	{
		mGameContext.window.draw(message.text);
	}
}

void MessageLogUI::registerToEvents()
{
	registerToLogMessageEvent();
}

void MessageLogUI::registerToLogMessageEvent()
{
	mGameContext.eventManager.registerEvent<LogMessageEvent>([this](const LogMessageEvent& data)
		{
			createMessage(data);
		});
}

void MessageLogUI::createMessageColors()
{
	mMessageColors[MessageType::GoldEarned]        = sf::Color{ 255, 215, 0}; //gold color
	mMessageColors[MessageType::PlayerDealtDamage] = sf::Color{ 255, 140, 0 }; // dark orange
	mMessageColors[MessageType::PlayerGotHit]      = sf::Color{ 220, 20,  60 }; //redish
	mMessageColors[MessageType::Custom]            = sf::Color{ 173, 216, 230 }; //light blue
}

void MessageLogUI::addTimeToMessages(const sf::Time& deltaTime)
{
	for (auto& message : mLogMessages)
		message.lifetime += deltaTime.asMilliseconds();
}

void MessageLogUI::createMessage(const LogMessageEvent& data)
{
	int messageCount = mLogMessages.size();
	if (messageCount == mMaxMessages)
		popLastMessage();

	shiftMessagesUp();
	initNewMessage(data);
	determineNewMessageString(data);
}

void MessageLogUI::popLastMessage()
{
	if (!mLogMessages.empty())
		mLogMessages.pop_back();
}

void MessageLogUI::shiftMessagesUp()
{
	const float shift = mMargin + mCharSize;
	for (auto& message : mLogMessages)
	{
		message.text.move({ 0.f, -shift });
	}
}

void MessageLogUI::initNewMessage(const LogMessageEvent& data)
{
	Message msg(mFont, data.type);
	msg.text.setCharacterSize(mCharSize);
	msg.text.setFillColor(mMessageColors[data.type]);
	msg.text.setPosition(mLogPosition);

	mLogMessages.push_front(std::move(msg));
}


void MessageLogUI::determineNewMessageString(const LogMessageEvent& data)
{
	auto& front = mLogMessages.front();
	if (data.type == MessageType::Custom && data.customMessage)
	{
		front.text.setString(data.customMessage.value());
		return;
	}
	std::string valueStr{};
	if (data.value)
		valueStr = std::to_string(data.value.value());
	std::string fullStr{};
	switch (data.type)
	{
	case MessageType::GoldEarned:
		fullStr = "Looted " + valueStr + " suashes!";
		break;
	case MessageType::PlayerDealtDamage:
		fullStr = "You strike for " + valueStr + ".";
		break;
	case MessageType::PlayerGotHit:
		fullStr = "Ouch! -" + valueStr + " HP.";
		break;
	default:
		break;
	}
	front.text.setString(fullStr);
}
