#include "ISystem.h"

class TileMap;
struct Tile;
class LevelAdvanceSystem
	:public ISystem
{
public:
	LevelAdvanceSystem(SystemContext& systemContext, const TileMap& tileMap, const sf::Font& font);
	// Inherited via ISystem
	virtual void processEvents(const sf::Event event);
	virtual void update(const sf::Time& deltaTime) override;
	virtual void render(sf::RenderWindow& window);
private:
	void registerToEvents();
	void registerToSetLevelAdvanceCellEvent();
	void registerToPlayerMovedEvent();

	const Tile* getAdvanceTile() const;
	bool isAdvanceTileVisible() const;
	bool isPlayerOnAdvanceCell() const;
	bool canPlayerAdvance() const;
	bool isAdvanceKeyPressed(const sf::Event event);
	void notifyAdvanceEvent();
	void createAdvanceText();
	void positionTextOnCellBottom();
	void updateAdvanceText(const sf::Time& deltaTime);

	void sendAdvanceMessage();

private:
	const TileMap& mTileMap;
	sf::Vector2i mLevelAdvanceCell;
	const sf::Keyboard::Key mAdvanceKey;
	sf::Text mAdvanceText;
	int mAdvanceTimer;
};