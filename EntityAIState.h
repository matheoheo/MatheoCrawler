#pragma once

enum class EntityAIState
{
	None,
	Patrolling,
	Chasing,
	Attacking,

	RepositionToAttack,
	Retreat
};