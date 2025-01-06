#include <stdio.h>

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

	state.worldDef = def->worldDef;
	state.pocketDef = def->pocketDef;
	state.puckPhysicsDef = def->puckPhysicsDef;
	state.strikerPhysicsDef = def->strikerPhysicsDef;
	state.strikerLimitDef = def->strikerLimitDef;

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

	// pockets
	{
		state.numOfPockets = def->numOfPockets;
		for (int i = 0; i < def->numOfPockets; i++)
		{
			const CarromPocketPositionDef posDef = def->pocketsPositions[i];
			const b2Circle circle = {posDef.position, def->pocketDef.radius};
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.isSensor = true;
			state.pockets[i] = b2CreateCircleShape(state.wallBodyId, &shapeDef, &circle);
		}
	}

	// striker
	{
		b2BodyDef strikerDef = b2DefaultBodyDef();
		strikerDef.type = b2_dynamicBody;
		strikerDef.isBullet = true;
		strikerDef.linearDamping = def->strikerPhysicsDef.bodyLinearDamping;
		strikerDef.angularDamping = def->strikerPhysicsDef.bodyAngularDamping;
		strikerDef.position = b2Vec2_zero;
		strikerDef.isEnabled = false;

		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.friction = def->strikerPhysicsDef.shapeFriction;
		shapeDef.restitution = def->strikerPhysicsDef.shapeRestitution;
		shapeDef.density = def->strikerPhysicsDef.shapeDensity;

		state.strikerBodyId = b2CreateBody(state.worldId, &strikerDef);

		b2Circle circle = {{0.0f, 0.0f}, def->strikerPhysicsDef.radius};
		b2CreateCircleShape(state.strikerBodyId, &shapeDef, &circle);
	}

	return state;
}

void CarromGameState_Step(const CarromGameState* state)
{
	// MACARON_ASSERT(state != NULL);
	b2World_Step(state->worldId, state->worldDef.frameDuration, state->worldDef.subStep);
}

typedef struct
{
	bool found;
	float minDistance;
	b2Vec2 nearestPos;
} FindNearestPuckData;

FindNearestPuckData CarromGameState_FindNearestPuck(const CarromGameState* state, const CarromPuck* target,
                                                    const b2Vec2 pos)
{
	float minDistance = state->worldDef.width + state->worldDef.height;
	FindNearestPuckData result = {false, minDistance, b2Vec2_zero};

	for (int i = 0; i < state->numOfPucks; i++)
	{
		const CarromPuck* puck = &state->pucks[i];
		if (puck == target || !b2Body_IsEnabled(puck->bodyId))
		{
			continue;
		}

		const b2Vec2 puckPos = b2Body_GetPosition(puck->bodyId);
		const float distance = b2Distance(puckPos, pos);
		if (distance < minDistance)
		{
			minDistance = distance;
			result.found = true;
			result.nearestPos = puckPos;
			result.minDistance = distance;
		}
	}

	return result;
}

b2Vec2 CarromGameState_PlacePuck(const CarromGameState* state, const CarromPuck* puck, const float step)
{
	const b2Vec2 targetPos = b2Body_GetPosition(puck->bodyId);
	const FindNearestPuckData nearestData = CarromGameState_FindNearestPuck(state, puck, targetPos);

	if (!nearestData.found)
	{
		// printf("CarromGameState_PlacePuck cannot find nearest puck\n");
		return targetPos;
	}

	const float r = state->puckPhysicsDef.radius;
	if (nearestData.minDistance > r * 2)
	{
		// printf("CarromGameState_PlacePuck no overlaps\n");
		return targetPos;
	}

	const b2Circle circleSrc = {{0.0f, 0.0f}, r};
	const b2Circle circleDst = {{0.0f, 0.0f}, r};

	const b2Transform transformSrc = {targetPos, b2Rot_identity};
	const b2Transform transformDst = {nearestData.nearestPos, b2Rot_identity};

	const b2Manifold m = b2CollideCircles(&circleSrc, transformSrc, &circleDst, transformDst);
	if (m.pointCount > 0)
	{
		const float pushOut = (r * 2) - nearestData.minDistance + 0.001f;
		b2Vec2 sepVec = b2MulSV(pushOut, b2Normalize(m.normal));

		if (pushOut < 0.00001f || b2Length(sepVec) < 0.00001f)
		{
			sepVec.x = 1.0f;
			sepVec.y = 0.0f;
		}

		b2Body_SetTransform(puck->bodyId, b2Add(targetPos, sepVec), b2Rot_identity);

		const b2Vec2 stepVec = b2MulSV(step, b2Normalize(sepVec));

		while (true)
		{
			const b2Vec2 placePos = b2Body_GetPosition(puck->bodyId);
			const FindNearestPuckData data = CarromGameState_FindNearestPuck(state, puck, placePos);
			if (!data.found || data.minDistance > r * 2)
			{
				return placePos;
			}

			const b2Vec2 newPos = b2Add(placePos, stepVec);
			b2Body_SetTransform(puck->bodyId, newPos, b2Rot_identity);
		}
	}

	return targetPos;
}

b2Vec2 CarromGameState_PlacePuckToCenter(const CarromGameState* state, const int index)
{
	MACARON_ASSERT(index >= 0 && index < state->numOfPucks);

	const CarromPuck* puck = &state->pucks[index];

	b2Body_SetTransform(puck->bodyId, b2Vec2_zero, b2Rot_identity);

	if (!b2Body_IsEnabled(puck->bodyId))
	{
		return b2Vec2_zero;
	}

	return CarromGameState_PlacePuck(state, puck, 0.001f);
}

FindNearestPuckData CarromGameState_FindStrikerNearestPuck(const CarromGameState* state, const b2Vec2 pos)
{
	float minDistance = state->worldDef.width + state->worldDef.height;

	FindNearestPuckData result = {false, minDistance, b2Vec2_zero};

	for (int i = 0; i < state->numOfPucks; i++)
	{
		const CarromPuck* puck = &state->pucks[i];
		if (!b2Body_IsEnabled(puck->bodyId))
		{
			continue;
		}

		const b2Vec2 puckPos = b2Body_GetPosition(puck->bodyId);
		const float distance = b2Distance(puckPos, pos);
		if (distance < minDistance)
		{
			minDistance = distance;
			result.found = true;
			result.nearestPos = puckPos;
			result.minDistance = distance;
		}
	}

	return result;
}

b2Vec2 CarromGameState_PlaceStriker(const CarromGameState* state, const CarromTablePosition tablePos, const float x)
{
	b2Vec2 pos = {x, 0.0f};
	if (tablePos == CarromTablePosition_Top)
	{
		pos.y = state->strikerLimitDef.centerOffset;
	}
	else
	{
		pos.y = -state->strikerLimitDef.centerOffset;
	}

	const float strikerLimitWidth = state->strikerLimitDef.width;
	const float left = -strikerLimitWidth / 2;
	const float right = strikerLimitWidth / 2;

	if (pos.x < left)
	{
		pos.x = left;
	}
	else if (pos.x > right)
	{
		pos.x = right;
	}

	b2Body_SetTransform(state->strikerBodyId, pos, b2Rot_identity);

	const FindNearestPuckData nearestData = CarromGameState_FindStrikerNearestPuck(state, pos);
	if (!nearestData.found)
	{
		return pos;
	}

	const float puckR = state->puckPhysicsDef.radius;
	const float strikerR = state->strikerPhysicsDef.radius;

	if (nearestData.minDistance > puckR + strikerR)
	{
		return pos;
	}

	// got overlap
	const b2Circle circleStriker = {{0.0f, 0.0f}, strikerR};
	const b2Circle circlePuck = {{0.0f, 0.0f}, puckR};

	const b2Transform transformStriker = {pos, b2Rot_identity};
	const b2Transform transformPuck = {nearestData.nearestPos, b2Rot_identity};

	const b2Manifold m = b2CollideCircles(&circleStriker, transformStriker, &circlePuck, transformPuck);
	if (m.pointCount > 0)
	{
		const float pushOut = (strikerR + puckR) - nearestData.minDistance + 0.001f;
		const float normalX = m.normal.x;

		float sepX;
		float direction;

		if (normalX == 0.f)
		{
			direction = 1.0f;
		}
		else
		{
			direction = normalX > 0.0f ? 1.0f : -1.0f;
		}

		if (pushOut < 0.00001f || b2AbsFloat(normalX) < 0.00001f)
		{
			sepX = direction * 0.001f;
		}
		else
		{
			sepX = pushOut * normalX;
		}

		b2Vec2 after;
		after.x = pos.x + sepX;
		after.y = pos.y;

		if (after.x < left)
		{
			after.x = left;
		}
		else if (after.x > right)
		{
			after.x = right;
		}

		b2Body_SetTransform(state->strikerBodyId, after, b2Rot_identity);

		bool reversed = false;

		while (true)
		{
			const float stepX = 0.001f;
			const b2Vec2 placePos = b2Body_GetPosition(state->strikerBodyId);
			const FindNearestPuckData data = CarromGameState_FindStrikerNearestPuck(state, placePos);
			if (!data.found || data.minDistance > strikerR + puckR)
			{
				return placePos;
			}

			b2Vec2 newPos = placePos;
			newPos.x += stepX * direction;
			if (newPos.x < left)
			{
				newPos.x = left;
				if (reversed)
				{
					printf("CarromGameState_PlaceStriker reversed\n");
					return pos;
				}
				direction = -direction;
				reversed = true;
			}
			else if (newPos.x > right)
			{
				newPos.x = right;
				if (reversed)
				{
					printf("CarromGameState_PlaceStriker reversed\n");
					return pos;
				}
				direction = -direction;
				reversed = true;
			}
			b2Body_SetTransform(state->strikerBodyId, newPos, b2Rot_identity);
		}
	}

	return pos;
}

void CarromGameState_Destroy(CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	b2DestroyWorld(state->worldId);
	state->worldId = b2_nullWorldId;
}
