#include <macaron/types.h>
#include <macaron/macaron.h>

#include "core.h"

CarromPuck CarromPuck_New(const b2WorldId worldId, const CarromPuckPhysicsDef* physicsDef)
{
	CarromPuck puck = {0};
	MACARON_ASSERT(physicsDef != NULL);
	if (physicsDef == NULL)
	{
		return puck;
	}

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = b2_dynamicBody;
	bodyDef.linearDamping = physicsDef->bodyLinearDamping;
	bodyDef.angularDamping = physicsDef->bodyAngularDamping;

	const b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

	b2ShapeDef shapeDef = b2DefaultShapeDef();
	shapeDef.friction = physicsDef->shapeFriction;
	shapeDef.restitution = physicsDef->shapeRestitution;
	shapeDef.density = physicsDef->shapeDensity;

	const b2Circle circle = {{0.0f, 0.0f}, physicsDef->radius};
	b2CreateCircleShape(bodyId, &shapeDef, &circle);

	puck.bodyId = bodyId;

	return puck;
}

CarromGameState CarromGameState_New(const CarromGameDef* def)
{
	CarromGameState state = {0};

	MACARON_ASSERT(def != NULL);
	if (def == NULL)
	{
		return state;
	}

	MACARON_ASSERT(def->numOfPucks > 0);

	// world

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.workerCount = def->worldDef.workerCount;
	worldDef.enableSleep = def->worldDef.enableSleep;
	worldDef.enableContinous = true;
	worldDef.gravity = b2Vec2_zero;

	state.worldId = b2CreateWorld(&worldDef);

	// pucks
	for (int i = 0; i < def->numOfPucks; i++)
	{
		CarromPuck puck = CarromPuck_New(state.worldId, &def->puckPhysicsDef);
		state.pucks[i] = puck;
		const CarromPuckPositionDef posDef = def->pucksPositions[i];
		puck.color = posDef.color;
		puck.index = posDef.index;
		puck.enabled = true;
		puck.originPos = posDef.position;
		CarromPuck_SetPosition(&puck, posDef.position);
	}

	return state;
}
