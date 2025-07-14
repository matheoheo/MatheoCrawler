#pragma once
#include "IState.h"
#include "TextButton.h"
#include "OptionSelector.h"

class OptionsState :
    public IState
{
public:
    OptionsState(GameContext& gameContext);
    // Inherited via IState
    virtual void onEnter() override;
    virtual void processEvents(sf::Event event) override;
    virtual void update(const sf::Time& deltaTime) override;
    virtual void render() override;
private:
    void createBackButton();
    void createBackground();
    void createHeaderText();
    void createApplyChangesText();
private:
    enum class OptionID
    {
        Resolution,
        Fullscreen,
        VSync,
        AntiAliasing
    };
    std::string optionIDToString(OptionID id) const;
    void createOptionIdToStringMap();
    void createOptions();
    void createResolutionOptionValues();
    void createAntiAliasingOptionValues();
    void createBooleanValues(OptionSelector& option);
    std::string getResolutionValue(const sf::Vector2u& res) const;

    void positionOptions();
private:
    const unsigned int mCharSize;
    const float mMarginPercent;
    TextButton mBackButton;
    sf::Sprite mBackground;
    sf::Text mHeaderText;
    sf::Text mApplyChangesText;
    std::unordered_map<OptionID, std::string> mOptionIdToStringMap;
    std::unordered_map<OptionID, OptionSelector> mOptions;
};

