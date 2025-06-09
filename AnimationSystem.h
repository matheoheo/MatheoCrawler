#pragma once
#include "ISystem.h"
#include "AnimationHolder.h"

class AnimationSystem :
    public ISystem
{
public:
    AnimationSystem(SystemContext& systemContext, const AnimationHolder& animationHolder);

    // Inherited via ISystem
    virtual void update(const sf::Time& deltaTime) override;
private:
    void registerToEvents();
    void resetTimer(AnimationComponent& animationComponent) const;
    bool hasFrameDurationPassed(const AnimationComponent& animationComponent) const;
    void applyCurrentFrame(const Entity& entity, AnimationComponent& animationComponent) const;
    void updateFrame(Entity& entity, AnimationComponent& animationComponent);
    void finalizeAnimation(const Entity& entity, AnimationComponent& animationComponent);
    void applyDefaultFrame(const Entity& entiy, AnimationComponent& animationComponent) ;

    void removeFinishedEntities();
    void registerToPlayGenericWalkEvent();
    void registerToFinalizeAnimationEvent();
    void registerToEntitySpecificAnimationEvent();
private:
    const AnimationHolder& mAnimationHolder;
    std::vector<const Entity*> mFinishedEntities;
    
};

