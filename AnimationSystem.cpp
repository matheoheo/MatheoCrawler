#include "pch.h"
#include "AnimationSystem.h"
#include "Config.h"

AnimationSystem::AnimationSystem(SystemContext& systemContext, const AnimationHolder& animationHolder)
	:ISystem(systemContext),
	mAnimationHolder(animationHolder)
{
	registerToEvents();
}

void AnimationSystem::update(const sf::Time& deltaTime)
{
	for (const auto& entity : mTrackedEntities)
	{
		auto& animComp = entity->getComponent<AnimationComponent>();
		animComp.cTimer += deltaTime.asMilliseconds();
		if (hasFrameDurationPassed(animComp))
		{
			resetTimer(animComp);
			updateFrame(*entity, animComp);
		}
	}
	removeFinishedEntities();
}

void AnimationSystem::registerToEvents()
{
	registerToPlayGenericWalkEvent();
	registerToFinalizeAnimationEvent();
	registerToEntitySpecificAnimationEvent();
}

void AnimationSystem::resetTimer(AnimationComponent& animationComponent) const
{
	animationComponent.cTimer = 0.f;
}

bool AnimationSystem::hasFrameDurationPassed(const AnimationComponent& animationComponent) const
{
	return animationComponent.cTimer >= animationComponent.cFrameDuration;
}

void AnimationSystem::applyCurrentFrame(const Entity& entity, AnimationComponent& animationComponent) const
{
	assert(animationComponent.cFrames);
	assert(animationComponent.cCurrentIndex >= 0 && animationComponent.cCurrentIndex < animationComponent.cFrames->size());
	
	auto& sprite = entity.getComponent<SpriteComponent>().cSprite;
	const auto& frame = (*animationComponent.cFrames)[animationComponent.cCurrentIndex];
	sprite.setTextureRect(frame.frameRect);

	if (animationComponent.cApplyOffset)
	{
		sf::Vector2f mod = { 53.f, 30.f };
		sprite.setPosition(animationComponent.cStartPosition - frame.offset + mod);
	}
}


void AnimationSystem::updateFrame(Entity& entity, AnimationComponent& animationComponent) 
{
	++animationComponent.cCurrentIndex;
	if (animationComponent.cCurrentIndex >= animationComponent.cFrames->size())
		return finalizeAnimation(entity, animationComponent);

	applyCurrentFrame(entity, animationComponent);
}

void AnimationSystem::finalizeAnimation(const Entity& entity, AnimationComponent& animationComponent) 
{
	animationComponent.cCurrentIndex = 0;
	//applyCurrentFrame(entity, animationComponent);
	animationComponent.cTimer = 0.f;
	animationComponent.cFrames = nullptr;

	if (animationComponent.cApplyOffset)
		entity.getComponent<SpriteComponent>().cSprite.setPosition(animationComponent.cStartPosition);
	applyDefaultFrame(entity, animationComponent);
	entity.getComponent<EntityStateComponent>().cCurrentState = EntityState::Idle;

	mFinishedEntities.push_back(&entity);
}

void AnimationSystem::applyDefaultFrame(const Entity& entity, AnimationComponent& animationComponent)
{
	auto dir = entity.getComponent<DirectionComponent>().cCurrentDir;
	auto key = GenericAnimationKey{ .id = animationComponent.cDefaultAnimId, .dir = dir };

	const auto& frame = mAnimationHolder.get(key)[0];
	entity.getComponent<SpriteComponent>().cSprite.setTextureRect(frame.frameRect);
}

void AnimationSystem::removeFinishedEntities()
{
	for (const Entity* entity : mFinishedEntities)
		std::erase(mTrackedEntities, entity);
	mFinishedEntities.clear();
}

void AnimationSystem::registerToPlayGenericWalkEvent()
{
	mSystemContext.eventManager.registerEvent<PlayGenericWalkEvent>([this](const PlayGenericWalkEvent& data)
		{
			auto& animComp = data.entity.getComponent<AnimationComponent>();
			const auto& moveComp = data.entity.getComponent<MovementComponent>();
			GenericAnimationKey key{ .id = AnimationIdentifier::GenericWalk,
				.dir = data.entity.getComponent<DirectionComponent>().cCurrentDir };

			animComp.cCurrentIndex = 0;
			animComp.cTimer = 0.f;
			animComp.cCurrentId = key.id;
			animComp.cFrames = &mAnimationHolder.get(key);
			animComp.cStartPosition = data.entity.getComponent<SpriteComponent>().cSprite.getPosition();
			animComp.cFrameDuration = ((Config::getCellSize().x - 3.5f)/ moveComp.cMoveSpeed / animComp.cFrames->size()) * 1000.f;
			animComp.cApplyOffset = false;
			applyCurrentFrame(data.entity, animComp);

			mTrackedEntities.push_back(&data.entity);
		});
}

void AnimationSystem::registerToFinalizeAnimationEvent()
{
	mSystemContext.eventManager.registerEvent<FinalizeAnimationEvent>([this](const FinalizeAnimationEvent& data)
		{
			finalizeAnimation(data.entity, data.entity.getComponent<AnimationComponent>());
		});
}

void AnimationSystem::registerToEntitySpecificAnimationEvent()
{
	mSystemContext.eventManager.registerEvent<PlayEntitySpecificAnimationEvent>([this](const PlayEntitySpecificAnimationEvent& data)
		{
			auto& animComp = data.entity.getComponent<AnimationComponent>();
			if (animComp.cFrames)
				return;
			data.entity.getComponent<EntityStateComponent>().cCurrentState = EntityState::Attacking;

			animComp.cCurrentIndex = 0;
			animComp.cTimer = 0.f;
			animComp.cFrameDuration = 750.f;
			animComp.cFrames = &mAnimationHolder.get(data.key);
			animComp.cStartPosition = data.entity.getComponent<SpriteComponent>().cSprite.getPosition();
			animComp.cApplyOffset = true;

			applyCurrentFrame(data.entity, animComp);
			mTrackedEntities.push_back(&data.entity);
		});
}
