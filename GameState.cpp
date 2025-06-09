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

GameState::GameState(GameContext& gameContext)
	:IState(gameContext),
	mGameView({Config::fWindowSize.x * 0.5f, Config::fWindowSize.y * 0.5f}, Config::fWindowSize),
	mTileMap(gameContext.eventManager, mGameView),
	mSystemContext(gameContext.eventManager, mEntityManager),
	mPathfinder(mTileMap),
	mBehaviorContext(gameContext.eventManager, mEntityManager, mTileMap)
{
	std::cout << "Welcome to game state!\n";
	onEnter();
}

void GameState::onEnter()
{
	loadTextures();
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
		/*auto enemies = mEntityManager.getEntitiesWithComponents<EnemyComponent>();
		auto& enemy = enemies[0];
		auto& dirComp = enemy->getComponent<DirectionComponent>();
		//mGameContext.eventManager.notify<MoveRequestedEvent>(MoveRequestedEvent(*enemy, Direction::Bottom));
		EntityAnimationKey key;
		key.dir = enemy->getComponent<DirectionComponent>().cCurrentDir;
		key.id = AnimationIdentifier::Attack1;
		key.type = EntityType::SkeletonAxe;

		mSystemContext.eventManager.notify<PlayEntitySpecificAnimationEvent>(PlayEntitySpecificAnimationEvent(*enemy, key));
		randomClock.restart();

		if (dirComp.cCurrentDir == Direction::Up)
			dirComp.cCurrentDir = Direction::Left;

		else if (dirComp.cCurrentDir == Direction::Left)
			dirComp.cCurrentDir = Direction::Bottom;

		else if (dirComp.cCurrentDir == Direction::Bottom)
			dirComp.cCurrentDir = Direction::Right;
		else if(dirComp.cCurrentDir == Direction::Right)
			dirComp.cCurrentDir = Direction::Up;
		//mTileMap.calculateVisibleTiles(mGameView.getCenter());*/
	}
	//mEntityManager.printSize();
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

	/*for (const auto& e : mEntityManager.getEntitiesWithComponents<SpriteComponent>())
	{
		mGameContext.window.draw(e->getComponent<SpriteComponent>().cSprite);
	}*/
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

void GameState::loadTextures()
{
	mGameContext.textures.load(TextureIdentifier::Player, "assets/entities/player/player.png");
	mGameContext.textures.load(TextureIdentifier::SkeletonAxe, "assets/entities/skeleton_axe/skeleton.png");
}

void GameState::createPlayer()
{
	auto& player = mEntityManager.createEntity();
	player.addComponent<MovementComponent>(150.f);
	player.addComponent<PlayerComponent>();
	player.addComponent<TagComponent>("PLAYER_DEFAULT");
	auto& spriteComp = player.addComponent<SpriteComponent>(mGameContext.textures.get(TextureIdentifier::Player));
	spriteComp.cSprite.setPosition(mTileMap.getFirstWalkablePos());
	spriteComp.cSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));

	player.addComponent<DirectionComponent>();
	player.addComponent<EntityStateComponent>();
	player.addComponent<AnimationComponent>();
	player.addComponent<FieldOfViewComponent>(5);

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
	mSystemManager.addSystem(std::make_unique<PathRequestSystem>(mSystemContext, mPathfinder));
	mSystemManager.addSystem(std::make_unique<BehaviorAIUpdateSystem>(mSystemContext, mTileMap));

	mSystemManager.addSystem(std::make_unique<TileFadeSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<EntityRenderSystem>(mSystemContext));
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
	sf::Vector2f pos{ cellIndex.x * Config::getCellSize().x, cellIndex.y * Config::getCellSize().y };

	auto& entity = mEntityManager.createEntity();
	entity.addComponent<MovementComponent>(100.f);
	entity.addComponent<TagComponent>("Skletorus");
	auto& spriteComp = entity.addComponent<SpriteComponent>(mGameContext.textures.get(TextureIdentifier::SkeletonAxe));
	spriteComp.cSprite.setPosition(pos);
	spriteComp.cSprite.setTextureRect(sf::IntRect({ 0, 0 }, { 64, 64 }));
	int cd = Random::get(2500, 4500);
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
	entity.addComponent<AttackRangeComponent>();
	entity.addComponent<AITimersComponent>();
	entity.addComponent<BehaviorComponent>(std::make_unique<BasicMeleeBehavior>(mBehaviorContext));
	mTileMap.getTiles()[cellIndex.y][cellIndex.x].occupyingEntities.push_back(&entity);
	//mGameContext.eventManager.notify<StartPatrollingEvent>(StartPatrollingEvent(entity));
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


