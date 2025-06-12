#include "pch.h"
#include "GameState.h"
#include "Config.h"
#include "Random.h"
#include "CameraSystem.h"
#include "InputSystem.h"
#include "CollisionSystem.h"
#include "MovementSystem.h"
#include "AnimationSystem.h"
#include "VisibilitySystem.h"
#include "PatrolAISystem.h"
#include "ChaseAISystem.h"
#include "PathRequestSystem.h"
#include "EntityRenderSystem.h"
#include "BehaviorAIUpdateSystem.h"
#include "TileFadeSystem.h"
#include "BasicMeleeBehavior.h"
#include "AttackSystem.h"
#include "OnHitSystem.h"
#include "BarRenderSystem.h"
#include "EntityDeathSystem.h"
#include "EntitySpawnerSystem.h"

GameState::GameState(GameContext& gameContext)
	:IState(gameContext),
	mGameView({Config::fWindowSize.x * 0.5f, Config::fWindowSize.y * 0.5f}, Config::fWindowSize),
	mTileMap(gameContext.eventManager, mGameView, mPathfinder),
	mSystemContext(gameContext.eventManager, mEntityManager),
	mPathfinder(mTileMap),
	mBehaviorContext(gameContext.eventManager, mEntityManager, mTileMap)
{
	std::cout << "Welcome to game state!\n";
	onEnter();
}

void GameState::onEnter()
{
	loadAnimations();
	createMap();
	createSystems();
	createPlayer();
	initalizePathfinder();
	spawnEntities();
}

void GameState::processEvents(sf::Event event)
{
	mSystemManager.processEvents(event);
}

void GameState::update(const sf::Time& deltaTime)
{
	IState::updateMousePosition();

	mSystemManager.update(deltaTime);

	static sf::Clock randomClock;
	if (randomClock.getElapsedTime().asMilliseconds() > 1000.f)
	{
	}
	static bool f = false;
	if (!f)
	{
		auto& spriteComp = mEntityManager.getPlayer().getComponent<SpriteComponent>();
		mGameContext.eventManager.notify<PlayerMoveFinishedEvent>(PlayerMoveFinishedEvent(spriteComp.cSprite.getPosition()));
		f = true;
	}
}

void GameState::render()
{
	renderMap();
	mSystemManager.render(mGameContext.window);
}

void GameState::createMap()
{
	mTileMap.buildFromGenerator(mGenerator, sf::Vector2i(100, 100), 5, sf::Vector2i(6, 6), sf::Vector2i(18, 18));
}

void GameState::renderMap()
{
	mGameContext.window.setView(mGameView);
	mTileMap.render(mGameContext.window);
}

void GameState::createPlayer()
{
	//placeholder function
	//ToDo: SpawnerSystem.
	auto& player = mEntityManager.createEntity();

	auto& attackComp = player.addComponent<AttackComponent>();

	AttackData att;
	att.hitOffsets.emplace(Direction::Up, std::vector<sf::Vector2i>());
	att.hitOffsets.emplace(Direction::Left, std::vector<sf::Vector2i>());
	att.hitOffsets.emplace(Direction::Bottom, std::vector<sf::Vector2i>());
	att.hitOffsets.emplace(Direction::Right, std::vector<sf::Vector2i>());

	att.hitOffsets.at(Direction::Up).emplace_back(0, -1);
	att.hitOffsets.at(Direction::Left).emplace_back(-1, 0);
	att.hitOffsets.at(Direction::Bottom).emplace_back(0, 1);
	att.hitOffsets.at(Direction::Right).emplace_back(1, 0);

	attackComp.cAttackDataMap.emplace(AnimationIdentifier::Attack1, std::move(att));

	player.addComponent<HealthBarComponent>();

}

void GameState::createSystems()
{
	mSystemManager.addSystem(std::make_unique<CameraSystem>(mSystemContext, mGameView));
	mSystemManager.addSystem(std::make_unique<InputSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<CollisionSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<MovementSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<AnimationSystem>(mSystemContext, mAnimationHolder));
	mSystemManager.addSystem(std::make_unique<VisibilitySystem>(mSystemContext, mTileMap, mGameView));
	mSystemManager.addSystem(std::make_unique<PatrolAISystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<ChaseAISystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<AttackSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<PathRequestSystem>(mSystemContext, mPathfinder));
	mSystemManager.addSystem(std::make_unique<BehaviorAIUpdateSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<OnHitSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<TileFadeSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<EntitySpawnerSystem>(mSystemContext, mGameContext.textures));

	mSystemManager.addSystem(std::make_unique<BarRenderSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<EntityRenderSystem>(mSystemContext));

	mSystemManager.addSystem(std::make_unique<EntityDeathSystem>(mSystemContext));
}

void GameState::loadAnimations()
{
	mAnimationHolder.loadAnimations();
}

void GameState::initalizePathfinder()
{
	mPathfinder.setSolidTypes({ TileType::Wall, TileType::None });
	mPathfinder.initalize();
}

void GameState::createSkeletonAxe(const sf::Vector2i& cellIndex)
{
	//placeholder function
	//ToDo: SpawnerSystem
	sf::Vector2f pos{ cellIndex.x * Config::getCellSize().x, cellIndex.y * Config::getCellSize().y };

	auto& entity = mEntityManager.createEntity();
	entity.addComponent<MovementComponent>(100.f);
	entity.addComponent<TagComponent>("Skletorus");
	auto& spriteComp = entity.addComponent<SpriteComponent>(mGameContext.textures.get(TextureIdentifier::SkeletonAxe));
	spriteComp.cSprite.setPosition(pos);
	spriteComp.cSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));
	int cd = Random::get(1500, 2200);
	float fcd = static_cast<float>(cd);
	entity.addComponent<DirectionComponent>();
	entity.addComponent<EntityStateComponent>();
	entity.addComponent<AnimationComponent>();
	entity.addComponent<FieldOfViewComponent>(5);
	entity.addComponent<EnemyComponent>();
	entity.addComponent<EntityAIStateComponent>();
	entity.addComponent<PatrolAIComponent>(fcd);
	entity.addComponent<ChaseAIComponent>();
	entity.addComponent<PathComponent>();
	entity.addComponent<EntityTypeComponent>(EntityType::SkeletonAxe);
	entity.addComponent<CombatStatsComponent>();
	entity.addComponent<AITimersComponent>();
	entity.addComponent<BehaviorComponent>(std::make_unique<BasicMeleeBehavior>(mBehaviorContext));
	auto& attackComp = entity.addComponent<AttackComponent>();
	mTileMap.getTiles()[cellIndex.y][cellIndex.x].occupyingEntities.push_back(&entity);
	AttackData att;
	att.hitOffsets.emplace(Direction::Up, std::vector<sf::Vector2i>()); 
	att.hitOffsets.emplace(Direction::Left, std::vector<sf::Vector2i>());
	att.hitOffsets.emplace(Direction::Bottom, std::vector<sf::Vector2i>());
	att.hitOffsets.emplace(Direction::Right, std::vector<sf::Vector2i>());

	att.hitOffsets.at(Direction::Up).emplace_back(0, -1);
	att.hitOffsets.at(Direction::Left).emplace_back(-1, 0);
	att.hitOffsets.at(Direction::Bottom).emplace_back(0, 1);
	att.hitOffsets.at(Direction::Right).emplace_back(1, 0);

	attackComp.cAttackDataMap.emplace(AnimationIdentifier::Attack1, std::move(att));

	entity.addComponent<HealthBarComponent>();
}

void GameState::spawnEntities()
{
	const auto& spawnPoints = mGenerator.getSpawnPoints();
	for (const auto& point : spawnPoints)
	{
		createSkeletonAxe(point);
	}
	std::cout << "Spawned: " << spawnPoints.size() << " entities\n";
}


