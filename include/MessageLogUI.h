#pragma once
#include "UIComponent.h"
#include "MessageTypes.h"

class MessageLogUI :
    public UIComponent
{
public:
    MessageLogUI(GameContext& gameContext, Entity& player);
    // Inherited via UIComponent
    virtual void processEvents(const sf::Event event) override;
    virtual void update(const sf::Vector2f& mousePosition, const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    struct Message {
        MessageType type;
        int lifetime = 0;
        sf::Text text;

        Message(const sf::Font& font, MessageType type)
            :text(font),
            type(type){}
    };
    void registerToEvents();
    void registerToLogMessageEvent();
    void createMessageColors();
    void addTimeToMessages(const sf::Time& deltaTime);
    void createMessage(const LogMessageEvent& data);

    void popLastMessage();
    void shiftMessagesUp();
    void initNewMessage(const LogMessageEvent& data);
    void determineNewMessageString(const LogMessageEvent& data);
private:
    const sf::Font& mFont;
    const int mMaxMessages;
    const int mLifetimeLimit; //in milliseconds
    const unsigned int mCharSize;
    const float mMargin;
    const sf::Vector2f mLogPosition; //position of the lowest message
    std::unordered_map<MessageType, sf::Color> mMessageColors;
    std::deque<Message> mLogMessages;
};

