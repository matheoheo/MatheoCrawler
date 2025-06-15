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
#include "Utilities.h"

GameState::GameState(GameContext& gameContext)
	:IState(gameContext),
	mGameView({Config::fWindowSize.x * 0.5f, Config::fWindowSize.y * 0.5f}, Config::fWindowSize),
	mTileMap(gameContext.eventManager, mGameView, mPathfinder),
	mSystemContext(gameContext.eventManager, mEntityManager),
	mPathfinder(mTileMap),
	mBehaviorContext(gameContext.eventManager, mEntityManager, mTileMap),
	mUIManager(gameContext)
{
	onEnter();
}

void GameState::onEnter()
{
	loadAnimations();
	createMap();
	createSystems();
	initalizePathfinder();
	spawnPlayer();
	spawnEntities();
	initalizeUI();
}

void GameState::processEvents(sf::Event event)
{
	mSystemManager.processEvents(event);
	mUIManager.processEvents(event);
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

	mUIManager.update(deltaTime);
}

void GameState::render()
{
	mGameContext.window.setView(mGameView);
	renderMap();
	mSystemManager.render(mGameContext.window);

	mUIManager.render();
}

void GameState::createMap()
{
	mTileMap.buildFromGenerator(mGenerator, sf::Vector2i(100, 100), 5, sf::Vector2i(6, 6), sf::Vector2i(18, 18));
}

void GameState::renderMap()
{
	mTileMap.render(mGameContext.window);
}

void GameState::spawnPlayer()
{
	auto playerPos = mTileMap.getFirstWalkablePos();
	auto playerCell = Utilities::getCellIndex(playerPos);
	mGameContext.eventManager.notify<SpawnEntityEvent>(SpawnEntityEvent(playerCell, EntityType::Player));
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
	mSystemManager.addSystem(std::make_unique<EntitySpawnerSystem>(mSystemContext, mGameContext.textures, mBehaviorContext));

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

void GameState::spawnEntities()
{
	const auto& spawnPoints = mGenerator.getSpawnPoints();
	for (const auto& point : spawnPoints)
	{
		mGameContext.eventManager.notify<SpawnEntityEvent>(SpawnEntityEvent(point, EntityType::Skletorus));
		//createSkeletonAxe(point);
	}
	std::cout << "Spawned: " << spawnPoints.size() << " entities\n";
}

void GameState::initalizeUI()
{
	mUIManager.setPlayer(&mEntityManager.getPlayer());
	mUIManager.createUI();
}


