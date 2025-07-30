#include "pch.h"
#include "AnimationHolder.h"

AnimationHolder::AnimationHolder()
{
}
void AnimationHolder::loadAnimations()
{
    loadGenericAnimations();

    loadPlayerSpecificAnimations();
    loadSkletorusSpecificAnimations();
    loadBonvikSpecificAnimations();
    loadMorannaSpecificAnimations();
}

const AnimationHolder::AnimationFrames& AnimationHolder::get(const GenericAnimationKey& key) const
{
    auto keyTmp = key;
    //Hurt animation has only 1 direction, so we have to make a special case
    if (keyTmp.id == AnimationIdentifier::GenericHurt)
        keyTmp.dir = Direction::Bottom;

    assert(mGenericAnimations.contains(keyTmp));
    return mGenericAnimations.at(keyTmp);
}

const AnimationHolder::AnimationFrames& AnimationHolder::get(const EntityAnimationKey& key) const
{
    assert(mEntityAnimations.contains(key));
    return mEntityAnimations.at(key);
}


void AnimationHolder::loadGenericAnimations()
{
    constexpr std::array<GenericAnimationKey, 21> idToGenericKey =
    { {
        { AnimationIdentifier::GenericSpellCast, Direction::Up },
        { AnimationIdentifier::GenericSpellCast, Direction::Left },
        { AnimationIdentifier::GenericSpellCast, Direction::Bottom },
        { AnimationIdentifier::GenericSpellCast, Direction::Right },

        { AnimationIdentifier::GenericThrustUnarmed, Direction::Up },
        { AnimationIdentifier::GenericThrustUnarmed, Direction::Left },
        { AnimationIdentifier::GenericThrustUnarmed, Direction::Bottom },
        { AnimationIdentifier::GenericThrustUnarmed, Direction::Right },

        { AnimationIdentifier::GenericWalk, Direction::Up },
        { AnimationIdentifier::GenericWalk, Direction::Left },
        { AnimationIdentifier::GenericWalk, Direction::Bottom },
        { AnimationIdentifier::GenericWalk, Direction::Right },

        { AnimationIdentifier::GenericSlashUnarmed, Direction::Up },
        { AnimationIdentifier::GenericSlashUnarmed, Direction::Left },
        { AnimationIdentifier::GenericSlashUnarmed, Direction::Bottom },
        { AnimationIdentifier::GenericSlashUnarmed, Direction::Right },

        { AnimationIdentifier::GenericShoot, Direction::Up },
        { AnimationIdentifier::GenericShoot, Direction::Left },
        { AnimationIdentifier::GenericShoot, Direction::Bottom },
        { AnimationIdentifier::GenericShoot, Direction::Right },

        { AnimationIdentifier::GenericHurt, Direction::Bottom }
    }};

    loadAnimationsFromFile<GenericAnimationKey>(mGenericAnimations, "assets/entities/genericAnimations.txt", 
        [this, &idToGenericKey](int animId) { return idToGenericKey[animId]; });
}

void AnimationHolder::loadPlayerSpecificAnimations()
{
    constexpr std::array<EntityAnimationKey, 12> idToEntityKey =
    {{
        {AnimationIdentifier::Attack1, Direction::Up,     EntityType::Player},
        {AnimationIdentifier::Attack1, Direction::Left,   EntityType::Player},
        {AnimationIdentifier::Attack1, Direction::Bottom, EntityType::Player},
        {AnimationIdentifier::Attack1, Direction::Right,  EntityType::Player},

        {AnimationIdentifier::Attack2, Direction::Up,     EntityType::Player},
        {AnimationIdentifier::Attack2, Direction::Left,   EntityType::Player},
        {AnimationIdentifier::Attack2, Direction::Bottom, EntityType::Player},
        {AnimationIdentifier::Attack2, Direction::Right,  EntityType::Player},

        {AnimationIdentifier::Attack3, Direction::Up,     EntityType::Player},
        {AnimationIdentifier::Attack3, Direction::Left,   EntityType::Player},
        {AnimationIdentifier::Attack3, Direction::Bottom, EntityType::Player},
        {AnimationIdentifier::Attack3, Direction::Right,  EntityType::Player},
    }};
    
    loadAnimationsFromFile<EntityAnimationKey>(mEntityAnimations, "assets/entities/player/animations.txt",
        [&idToEntityKey](int animId) {return idToEntityKey[animId]; });
}

void AnimationHolder::loadSkletorusSpecificAnimations()
{
    constexpr std::array<EntityAnimationKey, 4> idToEntityKey =
    { {
        {AnimationIdentifier::Attack1, Direction::Up,     EntityType::Skletorus},
        {AnimationIdentifier::Attack1, Direction::Left,   EntityType::Skletorus},
        {AnimationIdentifier::Attack1, Direction::Bottom, EntityType::Skletorus},
        {AnimationIdentifier::Attack1, Direction::Right,  EntityType::Skletorus}
    } };

    loadAnimationsFromFile<EntityAnimationKey>(mEntityAnimations, "assets/entities/skeleton_axe/animations.txt",
        [&idToEntityKey](int animId) {return idToEntityKey[animId]; });
}

void AnimationHolder::loadBonvikSpecificAnimations()
{
    constexpr std::array<EntityAnimationKey, 8> idToEntityKey =
    { {
        {AnimationIdentifier::Attack1, Direction::Up,     EntityType::Bonvik},
        {AnimationIdentifier::Attack1, Direction::Left,   EntityType::Bonvik},
        {AnimationIdentifier::Attack1, Direction::Bottom, EntityType::Bonvik},
        {AnimationIdentifier::Attack1, Direction::Right,  EntityType::Bonvik},
                                                                      
        {AnimationIdentifier::Attack2, Direction::Up,     EntityType::Bonvik},
        {AnimationIdentifier::Attack2, Direction::Left,   EntityType::Bonvik},
        {AnimationIdentifier::Attack2, Direction::Bottom, EntityType::Bonvik},
        {AnimationIdentifier::Attack2, Direction::Right,  EntityType::Bonvik}
    } };

    loadAnimationsFromFile<EntityAnimationKey>(mEntityAnimations, "assets/entities/Bonvik/animations.txt",
        [&idToEntityKey](int animId) {return idToEntityKey[animId]; });
}

void AnimationHolder::loadMorannaSpecificAnimations()
{
    constexpr std::array<EntityAnimationKey, 4> idToEntityKey =
    { {
        {AnimationIdentifier::Attack1, Direction::Up,     EntityType::Moranna},
        {AnimationIdentifier::Attack1, Direction::Left,   EntityType::Moranna},
        {AnimationIdentifier::Attack1, Direction::Bottom, EntityType::Moranna},
        {AnimationIdentifier::Attack1, Direction::Right,  EntityType::Moranna}
    } };

    loadAnimationsFromFile<EntityAnimationKey>(mEntityAnimations, "assets/entities/Moranna/animations.txt",
        [&idToEntityKey](int animId) {return idToEntityKey[animId]; });
}

AnimationFrame AnimationHolder::makeFrame(const FrameData& data) const
{
    sf::Vector2i framePos
    { 
        parseInt(data, "frameLeft"), 
        parseInt(data, "frameTop") 
    };
    sf::Vector2i frameSize
    {
        parseInt(data, "frameWidth"),
        parseInt(data, "frameHeight")
    };
    sf::Vector2f frameOffset
    {
        parseFloat(data, "frameOffsetX"),
        parseFloat(data, "frameOffsetY")
    };

    return AnimationFrame
    {
        .frameRect = sf::IntRect(framePos, frameSize),
        .offset = frameOffset
    };
}

int AnimationHolder::parseInt(const FrameData& data, const std::string& key, int defaultReturn) const
{
    if (auto it = data.find(key); it != std::end(data))
        return std::stoi(it->second);
    return defaultReturn;
}

float AnimationHolder::parseFloat(const FrameData& data, const std::string& key, float defaultReturn) const
{
    if (auto it = data.find(key); it != std::end(data))
        return std::stof(it->second);
    return defaultReturn;
}
