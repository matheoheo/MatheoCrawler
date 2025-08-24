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
#include "MessageTypes.h"
#include "RegenerationSystem.h"
#include "SpellSystem.h"
#include "HealSpellSystem.h"
#include "EffectSystem.h"
#include "ProjectileSystem.h"
#include "PositioningAISystem.h"
#include "PathFollowSystem.h"
#include "SpellEffectSystem.h"
#include "StatusIconDisplaySystem.h"
#include "AreaOfEffectSpellSystem.h"
#include "LevelAdvanceSystem.h"
#include "StateIdentifiers.h"
#include "GameStatisticsSystem.h"

GameState::GameState(GameContext& gameContext)
	:IState(gameContext),
	mGameView({ Config::fWindowSize.x * 0.5f, Config::fWindowSize.y * 0.5f }, Config::fWindowSize),
	mTileMap(gameContext.eventManager, mGameView, mPathfinder),
	mSystemContext(gameContext.eventManager, mEntityManager),
	mPathfinder(mTileMap),
	mBehaviorContext(gameContext.eventManager, mEntityManager, mTileMap),
	mUIManager(gameContext),
	mLoadNextLevel(false),
	mLevelLoaded(false),
	mProceedToMenu(false)
{
	registerToEvents();
}

void GameState::onEnter()
{
	doFirstEnter();
}

void GameState::processEvents(sf::Event event)
{
	mSystemManager.processEvents(event);
	mUIManager.processEvents(event);
}

void GameState::update(const sf::Time& deltaTime)
{
	mGameContext.window.setView(mGameView);
	IState::updateMousePosition();
	mSystemManager.update(deltaTime);
	mUIManager.update(deltaTime);

	if (!mLevelLoaded)
	{
		notifyMoveFinished();
		mLevelLoaded = true;
	}

	if (mLoadNextLevel)
	{
		mLoadNextLevel = false;
		loadNextLevel();
		return;
	}

	if (mProceedToMenu)
	{
		mProceedToMenu = false;
		mGameContext.eventManager.notify<SwitchStateEvent>(SwitchStateEvent(StateIdentifier::MenuState, true));
		return;
	}
}

void GameState::render()
{
	mGameContext.window.setView(mGameView);
	renderMap();
	mSystemManager.render(mGameContext.window);

	mUIManager.render();
}

void GameState::registerToEvents()
{
	registerToLoadNextLevelEvent();
	registerToProceedToMainMenuEvent();
}

void GameState::registerToLoadNextLevelEvent()
{
	mGameContext.eventManager.registerEvent<LoadNextLevelEvent>([this](const LoadNextLevelEvent& data)
		{
			mLoadNextLevel = true;
		});
}

void GameState::registerToProceedToMainMenuEvent()
{
	mGameContext.eventManager.registerEvent<ProceedToMainMenuEvent>([this](const ProceedToMainMenuEvent& data)
		{
			mProceedToMenu = true;
		});
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
	auto playerCell = mGenerator.getSpawnCell();
	mGameContext.eventManager.notify<SpawnEntityEvent>(SpawnEntityEvent(playerCell, EntityType::Player));
	//add player to regeneration system
	mGameContext.eventManager.notify<TriggerHpRegenSpellEvent>(TriggerHpRegenSpellEvent(mEntityManager.getPlayer(), 0, 0));
}

void GameState::positionPlayer()
{
	//This function is getting called when loading to the next level of dungeon.

	auto& player = mEntityManager.getPlayer();
	sf::Vector2f validPos = { 
		mGenerator.getSpawnCell().x * Config::getCellSize().x,
		mGenerator.getSpawnCell().y * Config::getCellSize().y 
	};
	player.getComponent<SpriteComponent>().cSprite.setPosition(validPos);
	player.getComponent<SpellEffectsComponent>().cActiveEffects.clear();
	mGameContext.eventManager.notify<TileOccupiedEvent>(TileOccupiedEvent(player, validPos));
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
	mSystemManager.addSystem(std::make_unique<PositioningAISystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<AttackSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<PathRequestSystem>(mSystemContext, mPathfinder));
	mSystemManager.addSystem(std::make_unique<BehaviorAIUpdateSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<OnHitSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<TileFadeSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<EntitySpawnerSystem>(mSystemContext, mGameContext.textures, mBehaviorContext));
	mSystemManager.addSystem(std::make_unique<RegenerationSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<SpellSystem>(mSystemContext, mTileMap, fMousePos));
	mSystemManager.addSystem(std::make_unique<HealSpellSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<EffectSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<ProjectileSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<PathFollowSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<SpellEffectSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<AreaOfEffectSpellSystem>(mSystemContext, mTileMap));
	mSystemManager.addSystem(std::make_unique<StatusIconDisplaySystem>(mSystemContext, mGameContext.textures));
	mSystemManager.addSystem(std::make_unique<GameStatisticsSystem>(mSystemContext));
	mSystemManager.addSystem(std::make_unique<LevelAdvanceSystem>(mSystemContext, mTileMap, mGameContext.fonts.get(FontIdentifiers::UIFont)));

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
	mPathfinder.initalize();
}

void GameState::spawnEntities()
{
	const auto& spawnPoints = mGenerator.getSpawnPoints();
	mGameContext.eventManager.notify<GenerateEntitiesEvent>(GenerateEntitiesEvent(spawnPoints));
}

void GameState::initalizeUI()
{
	mUIManager.setPlayer(&mEntityManager.getPlayer());
	mUIManager.createUI();
}

void GameState::logOnEnterMessage()
{
	auto lvl = Config::difficulityLevel;
	if(lvl == 1)
		mGameContext.eventManager.notify<LogMessageEvent>(LogMessageEvent(MessageType::Custom, 0, "Welcome to MattCrawler!"));
	std::string msg = "You descend to floor " + std::to_string(lvl);
	mGameContext.eventManager.notify<LogMessageEvent>(LogMessageEvent(MessageType::Custom, 0, msg));
}

void GameState::doFirstEnter()
{
	std::vector<std::function<void()>> tasks;
	tasks.push_back([this]() { loadAnimations(); });
	tasks.push_back([this]() { createMap(); });
	tasks.push_back([this]() { createSystems(); });
	tasks.push_back([this]() { initalizePathfinder(); });
	tasks.push_back([this]() { spawnPlayer(); });
	tasks.push_back([this]() { spawnEntities(); });
	tasks.push_back([this]() { notifySetLevelAdvancedCellEvent(); });
	tasks.push_back([this]() { initalizeUI(); });
	tasks.push_back([this]() { logOnEnterMessage(); });
	tasks.push_back([this]() {mEntityManager.getPlayer().getComponent<PlayerResourcesComponent>().cGold = 93219; });
	mGameContext.eventManager.notify<EnterLoadingStateEvent>(EnterLoadingStateEvent(std::move(tasks)));
}

void GameState::removeEntities()
{
	//we call to remove all entities
	//this is called when loading next level - we do not want old entities in our memory no more.
	for (const auto& ent : mEntityManager.getEntities())
	{
		if (ent->hasComponent<PlayerComponent>())
			continue;

		mGameContext.eventManager.notify<EntityDiedEvent>(EntityDiedEvent(*ent, false));
	}
}

void GameState::loadNextLevel()
{
	mGameContext.eventManager.notify<BeforeLoadNextLevelEvent>(BeforeLoadNextLevelEvent());
	++Config::difficulityLevel;
	std::vector<std::function<void()>> tasks;
	tasks.push_back([this]() { removeEntities(); }); 
	tasks.push_back([this]() { createMap(); });
	tasks.push_back([this]() { initalizePathfinder(); });
	tasks.push_back([this]() { positionPlayer(); });
	tasks.push_back([this]() { spawnEntities(); });
	tasks.push_back([this]() { notifySetLevelAdvancedCellEvent(); });
	tasks.push_back([this]() { logOnEnterMessage(); });
	tasks.push_back([this]() { mLevelLoaded = false; });

	mGameContext.eventManager.notify<EnterLoadingStateEvent>(EnterLoadingStateEvent(std::move(tasks)));
}

void GameState::notifyMoveFinished()
{
	//We simulate the movement, so the visibility gets updated.
	auto& spriteComp = mEntityManager.getPlayer().getComponent<SpriteComponent>();
	mGameContext.eventManager.notify<PlayerMoveFinishedEvent>(PlayerMoveFinishedEvent(spriteComp.cSprite.getPosition()));
}

void GameState::notifySetLevelAdvancedCellEvent()
{
	mGameContext.eventManager.notify<SetLevelAdvanceCellEvent>(SetLevelAdvanceCellEvent(mGenerator.getNextLevelCell()));
}

