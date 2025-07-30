#include "pch.h"
#include "GlowUpVisualEffect.h"
#include "Entity.h"
#include "Utilities.h"

GlowUpVisualEffect::GlowUpVisualEffect(const Entity& entity, int duration, const sf::Color& targetColor)
	:IVisualEffect(entity, duration),
	mSprite(entity.getComponent<SpriteComponent>().cSprite),
	mStartingColor(mSprite.getColor()),
	mTargetColor(targetColor)
{

}

void GlowUpVisualEffect::update(const sf::Time& deltaTime)
{
	addTimePassed(deltaTime);
	if (hasEffectDurationPassed())
	{
		onEffectFinish();
		return;
	}
	updateEffect(deltaTime);
}

void GlowUpVisualEffect::render(sf::RenderWindow& window)
{
}

void GlowUpVisualEffect::updateEffect(const sf::Time& deltaTime)
{
	constexpr float pi = std::numbers::pi_v<float>;
	float progress = getProgress();
	float wave = std::sin(progress * pi);
	auto newColor = Utilities::lerpColor(mStartingColor, mTargetColor, wave);
	mSprite.setColor(newColor);
}

void GlowUpVisualEffect::onEffectFinish()
{
	mSprite.setColor(mStartingColor);
}

float GlowUpVisualEffect::getProgress() const
{
	return static_cast<float>(mTimer) / static_cast<float>(mDuration);
}
