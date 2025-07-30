#include "pch.h"
#include "RegenerationVisualEffect.h"
#include "Entity.h"
#include "Random.h"
#include "Utilities.h"

RegenerationVisualEffect::RegenerationVisualEffect(const Entity& entity, int duration, const sf::Color& effectColor)
	:IVisualEffect(entity, duration),
	mEffectColor(effectColor),
	mEffectLifetimeLimit(90), //90 frames
	mLastSpawn(0)
{
}

void RegenerationVisualEffect::update(const sf::Time& deltaTime)
{
	addTimePassed(deltaTime);
	updateEffect(deltaTime);
}

void RegenerationVisualEffect::render(sf::RenderWindow& window)
{
	for (const auto& effect : mVisuals)
	{
		window.draw(effect.vertical);
		window.draw(effect.horizontal);
	}
}

void RegenerationVisualEffect::updateEffect(const sf::Time& deltaTime)
{
	for (auto& effect : mVisuals)
		updateRegenVisual(effect, deltaTime);

	if (isEffectSpawnDue())
		mVisuals.push_back(spawnVisualEffect(getSpawnPos()));

	++mLastSpawn;
	removeFinishedVisuals();
}

RegenerationVisualEffect::RegenVisual RegenerationVisualEffect::spawnVisualEffect(const sf::Vector2f& pos) const
{
	constexpr sf::Vector2f size{ 8.f, 3.f };
	constexpr int maxDir = 30;
	constexpr float yDirection = -1.0f;
	RegenVisual result;
	
	//So we do not want the effect to move only straight upwards.
	//We will get random direction vector for better visual effect.
	int randomDir = Random::get(-maxDir, maxDir);
	result.dirVector = { static_cast<float>(randomDir) / 100.f, yDirection };
	result.lifeTime = 0; 

	result.vertical.setSize({ size.y, size.x });
	result.vertical.setFillColor(mEffectColor);
	result.vertical.setPosition(pos);

	result.horizontal.setSize(size);
	result.horizontal.setFillColor(mEffectColor);
	result.horizontal.setOrigin(size * 0.5f);
	result.horizontal.setPosition(pos + result.vertical.getGeometricCenter());
	
	return result;
}

void RegenerationVisualEffect::updateRegenVisual(RegenVisual& effect, const sf::Time& deltaTime)
{
	constexpr float speed = 50.f;
	const sf::Vector2f moveVector{ deltaTime.asSeconds() * speed * effect.dirVector };
	effect.horizontal.move(moveVector);
	effect.vertical.move(moveVector);

	++effect.lifeTime;
	updateEffectTransparency(effect.horizontal);
	updateEffectTransparency(effect.vertical);
}

void RegenerationVisualEffect::updateEffectTransparency(sf::RectangleShape& rect)
{
	constexpr int alphaDecremental = 2;
	auto color = rect.getFillColor();
	if (color.a >= alphaDecremental)
		color.a -= alphaDecremental;
	else
		color.a = 0;

	rect.setFillColor(color);
}

bool RegenerationVisualEffect::hasRegenVisualLifetimePassed(const RegenVisual& effect)
{
	return effect.lifeTime >= mEffectLifetimeLimit;
}

void RegenerationVisualEffect::removeFinishedVisuals()
{
	std::erase_if(mVisuals, [this](const RegenVisual& effect) {
		return hasRegenVisualLifetimePassed(effect);
	});
}

bool RegenerationVisualEffect::isEffectSpawnDue() const
{
	return mLastSpawn % mEffectLifetimeLimit == 0;
}

sf::Vector2f RegenerationVisualEffect::getSpawnPos() const
{
	const float cellSize = Config::getCellSize().x;

	auto entCell = Utilities::getEntityCell(mEntity);
	sf::Vector2f cellPos{
		entCell.x * cellSize,
		entCell.y * cellSize
	};

	//So we want to get random spawn position, that is between [cellPos + 25% of cellSize] up to [cellPos + 75% of cellSize]
	constexpr float minModifierRatio = 0.25f;
	constexpr float maxModifierRatio = 0.75f;
	const int minMargin = static_cast<int>(cellSize * minModifierRatio);
	const int maxMargin = static_cast<int>(cellSize * maxModifierRatio);

	int minXPos = static_cast<int>(cellPos.x + minMargin);
	int maxXPos = static_cast<int>(cellPos.x + maxMargin);

	//same for Y axis
	int minYPos = static_cast<int>(cellPos.y + minMargin);
	int maxYPos = static_cast<int>(cellPos.y + maxMargin);

	//now we get random x and random y
	int x = Random::get(minXPos, maxXPos);
	int y = Random::get(minYPos, maxYPos);
	
	return
	{
		static_cast<float>(x),
		static_cast<float>(y)
	};
}
