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
    void createButtons();
    void createBackground();
    void createHeaderText();
    void createApplyChangesText();
    void applyChanges();
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
    void positionOptions();
    void setActualValuesToOptions();

    std::string resolutionToString(const sf::Vector2u& res) const;
    sf::Vector2u getResolutionValue() const;
    unsigned int getAntiAliasingValue() const;
private:
    const unsigned int mCharSize;
    const float mLeftMarginPercent;
    const float mRightMarginPercent;
    std::vector<TextButton> mButtons; //save & back button;
    sf::Sprite mBackground;
    sf::Text mHeaderText;
    sf::Text mApplyChangesText;
    std::unordered_map<OptionID, std::string> mOptionIdToStringMap;
    std::unordered_map<OptionID, OptionSelector> mOptions;
    bool mConfigSaved;
};

