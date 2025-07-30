#include "pch.h"
#include "UIComponent.h"
#include "Entity.h"

UIComponent::UIComponent(GameContext& gameContext, Entity& player)
	:mGameContext(gameContext),
	player(player)
{
}
