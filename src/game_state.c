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

	state.frameDuration = def->worldDef.frameDuration;
	state.subSteps = def->worldDef.subStep;

	// world
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.workerCount = def->worldDef.workerCount;
		worldDef.enableSleep = !def->worldDef.disableSleep;
		worldDef.enableContinous = true;
		worldDef.gravity = b2Vec2_zero;

		state.worldId = b2CreateWorld(&worldDef);
	}

	// table walls
	{
		const float w = def->worldDef.width;
		const float h = def->worldDef.height;

		b2Vec2 tablePoints[] = {{w / 2, -h / 2}, {-w / 2, -h / 2,}, {-w / 2, h / 2}, {w / 2, h / 2}};
		const b2BodyDef wallDef = b2DefaultBodyDef();
		state.wallBodyId = b2CreateBody(state.worldId, &wallDef);
		{
			b2ChainDef chainDef = b2DefaultChainDef();
			chainDef.points = tablePoints;
			chainDef.count = sizeof(tablePoints) / sizeof(b2Vec2);
			chainDef.isLoop = true;
			chainDef.friction = 0.0f;
			chainDef.restitution = 1.0f;
			b2CreateChain(state.wallBodyId, &chainDef);
		}
	}

	// pucks
	{
		state.numOfPucks = def->numOfPucks;
		for (int i = 0; i < def->numOfPucks; i++)
		{
			CarromPuck puck = CarromPuck_New(state.worldId, &def->puckPhysicsDef);
			const CarromPuckPositionDef posDef = def->pucksPositions[i];
			puck.color = posDef.color;
			puck.index = posDef.index;
			puck.originPos = posDef.position;
			CarromPuck_SetPosition(&puck, posDef.position);
			state.pucks[i] = puck;
		}
	}

	return state;
}

void CarromGameState_Step(const CarromGameState* state)
{
	// MACARON_ASSERT(state != NULL);
	b2World_Step(state->worldId, state->frameDuration, state->subSteps);
}

void CarromGameState_Destroy(CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	b2DestroyWorld(state->worldId);
	state->worldId = b2_nullWorldId;
}
