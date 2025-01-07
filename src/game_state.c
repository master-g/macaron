#include <macaron/types.h>
#include <macaron/macaron.h>

#include "core.h"
#include "../extern/box2d/src/body.h"

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

void CarromGameState_CreateImpl(CarromGameState* state,
                                const CarromWorldDef* def,
                                const CarromPocketDef* pocketDef,
                                const CarromPuckPhysicsDef* puckPhysicsDef,
                                const CarromPuckPhysicsDef* strikerPhysicsDef,
                                const CarromStrikerLimitDef* strikerLimitDef,
                                int numOfPucks,
                                const CarromPuckPositionDef* pucksPositions,
                                int numOfPockets,
                                const CarromPocketPositionDef* pocketsPositions)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(def != NULL);
	MACARON_ASSERT(pocketDef != NULL);
	MACARON_ASSERT(puckPhysicsDef != NULL);
	MACARON_ASSERT(strikerPhysicsDef != NULL);
	MACARON_ASSERT(strikerLimitDef != NULL);
	MACARON_ASSERT(pucksPositions != NULL);
	MACARON_ASSERT(pocketsPositions != NULL);
	MACARON_ASSERT(numOfPucks > 0);
	MACARON_ASSERT(numOfPockets > 0);

	if (state == NULL || def == NULL || pocketDef == NULL || puckPhysicsDef == NULL || strikerPhysicsDef == NULL ||
	    strikerLimitDef == NULL || pucksPositions == NULL || pocketsPositions == NULL || numOfPucks <= 0 ||
	    numOfPockets <= 0)
	{
		return;
	}

	state->worldDef = *def;
	state->pocketDef = *pocketDef;
	state->puckPhysicsDef = *puckPhysicsDef;
	state->strikerPhysicsDef = *strikerPhysicsDef;
	state->strikerLimitDef = *strikerLimitDef;

	// world
	{
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.workerCount = def->workerCount;
		worldDef.enableSleep = !def->disableSleep;
		worldDef.enableContinous = true;
		worldDef.gravity = b2Vec2_zero;

		state->worldId = b2CreateWorld(&worldDef);
	}

	// table walls
	{
		const float w = def->width;
		const float h = def->height;

		b2Vec2 tablePoints[] = {{w / 2, -h / 2}, {-w / 2, -h / 2,}, {-w / 2, h / 2}, {w / 2, h / 2}};
		const b2BodyDef wallDef = b2DefaultBodyDef();
		state->wallBodyId = b2CreateBody(state->worldId, &wallDef);
		{
			b2ChainDef chainDef = b2DefaultChainDef();
			chainDef.points = tablePoints;
			chainDef.count = sizeof(tablePoints) / sizeof(b2Vec2);
			chainDef.isLoop = true;
			chainDef.friction = 0.0f;
			chainDef.restitution = 1.0f;
			b2CreateChain(state->wallBodyId, &chainDef);
		}
	}

	// pucks
	{
		state->numOfPucks = numOfPucks;
		for (int i = 0; i < numOfPucks; i++)
		{
			CarromPuck puck = CarromPuck_New(state->worldId, puckPhysicsDef);
			const CarromPuckPositionDef posDef = pucksPositions[i];
			puck.index = i;
			puck.color = posDef.color;
			b2Body_SetTransform(puck.bodyId, posDef.position, b2Rot_identity);
			state->pucks[i] = puck;
			state->pucksPositionDefs[i] = posDef;

			b2Body_SetUserData(puck.bodyId, (void*)(intptr_t)i);
		}
	}

	// pockets
	{
		state->numOfPockets = numOfPockets;
		for (int i = 0; i < numOfPockets; i++)
		{
			const CarromPocketPositionDef posDef = pocketsPositions[i];
			const b2Circle circle = {posDef.position, pocketDef->radius};
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.isSensor = true;
			state->pockets[i] = b2CreateCircleShape(state->wallBodyId, &shapeDef, &circle);
			state->pocketsPositionDefs[i] = posDef;
		}
	}

	// striker
	{
		b2BodyDef strikerDef = b2DefaultBodyDef();
		strikerDef.type = b2_dynamicBody;
		strikerDef.isBullet = true;
		strikerDef.linearDamping = strikerPhysicsDef->bodyLinearDamping;
		strikerDef.angularDamping = strikerPhysicsDef->bodyAngularDamping;
		strikerDef.position = b2Vec2_zero;
		strikerDef.isEnabled = false;

		b2ShapeDef shapeDef = b2DefaultShapeDef();
		shapeDef.friction = strikerPhysicsDef->shapeFriction;
		shapeDef.restitution = strikerPhysicsDef->shapeRestitution;
		shapeDef.density = strikerPhysicsDef->shapeDensity;

		state->strikerBodyId = b2CreateBody(state->worldId, &strikerDef);

		b2Circle circle = {{0.0f, 0.0f}, strikerPhysicsDef->radius};
		b2CreateCircleShape(state->strikerBodyId, &shapeDef, &circle);
	}
}

CarromGameState CarromGameState_New(const CarromGameDef* def)
{
	MACARON_ASSERT(def != NULL);
	CarromGameState state = {0};
	if (def == NULL)
	{
		return state;
	}

	CarromGameState_CreateImpl(&state,
	                           &def->worldDef,
	                           &def->pocketDef,
	                           &def->puckPhysicsDef,
	                           &def->strikerPhysicsDef,
	                           &def->strikerLimitDef,
	                           def->numOfPucks,
	                           def->pucksPositions,
	                           def->numOfPockets,
	                           def->pocketsPositions);

	return state;
}

bool CarromGameState_HasMovement(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return false;
	}

	MACARON_ASSERT(b2World_IsValid(state->worldId));

	const b2BodyEvents bodyEvents = b2World_GetBodyEvents(state->worldId);
	if (bodyEvents.moveCount > 0)
	{
		return true;
	}

	return false;
}

bool CarromGameState_HasSensorEvents(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return false;
	}

	MACARON_ASSERT(b2World_IsValid(state->worldId));

	const b2SensorEvents sensorEvents = b2World_GetSensorEvents(state->worldId);
	if (sensorEvents.beginCount > 0 || sensorEvents.endCount > 0)
	{
		return true;
	}

	return false;
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

CarromPuckColor CarromGameState_GetPuckColor(const CarromGameState* state, const int index)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(index >= 0 && index < state->numOfPucks);
	if (state == NULL)
	{
		return CarromPuckColor_White;
	}

	const CarromPuck* puck = &state->pucks[index];
	return puck->color;
}

b2Vec2 CarromGameState_GetPuckPosition(const CarromGameState* state, const int index)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(index >= 0 && index < state->numOfPucks);

	const CarromPuck* puck = &state->pucks[index];
	return b2Body_GetPosition(puck->bodyId);
}

b2Vec2 CarromGameState_GetStrikerPosition(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return b2Vec2_zero;
	}

	return b2Body_GetPosition(state->strikerBodyId);
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

b2Vec2 CarromGameState_PlacePuckToPos(const CarromGameState* state, const int index, const b2Vec2 pos)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(index >= 0 && index < state->numOfPucks);
	if (state == NULL)
	{
		return b2Vec2_zero;
	}

	const CarromPuck* puck = &state->pucks[index];
	b2Body_SetTransform(puck->bodyId, pos, b2Rot_identity);

	const float step = state->puckPhysicsDef.radius / 200.0f;
	return CarromGameState_PlacePuck(state, puck, step);
}

void CarromGameState_PlacePuckToPosUnsafe(const CarromGameState* state, const int index, const b2Vec2 pos)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(index >= 0 && index < state->numOfPucks);

	const CarromPuck* puck = &state->pucks[index];
	b2Body_SetTransform(puck->bodyId, pos, b2Rot_identity);
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
					// printf("CarromGameState_PlaceStriker reversed\n");
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
					// printf("CarromGameState_PlaceStriker reversed\n");
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

bool CarromGameState_IsStrikerOverlapping(const CarromGameState* state, const b2Vec2 pos)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return false;
	}

	const FindNearestPuckData data = CarromGameState_FindStrikerNearestPuck(state, pos);

	const float strikerR = state->strikerPhysicsDef.radius;
	const float puckR = state->puckPhysicsDef.radius;

	if (data.found && data.minDistance < strikerR + puckR)
	{
		return true;
	}

	return false;
}

void CarromGameState_Strike(const CarromGameState* state, b2Vec2 impulse)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return;
	}

	const float force = b2Length(impulse);
	if (force > state->strikerLimitDef.maxForce)
	{
		const float scale = state->strikerLimitDef.maxForce / force;
		impulse = b2MulSV(scale, impulse);
	}
	if (!b2Body_IsEnabled(state->strikerBodyId))
	{
		b2Body_Enable(state->strikerBodyId);
	}
	b2Body_ApplyForceToCenter(state->strikerBodyId, impulse, true);
}

CarromSnapshot CarromGameState_TakeSnapshot(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(b2World_IsValid(state->worldId));

	CarromSnapshot snapshot = {0};
	for (int i = 0; i < state->numOfPucks; i++)
	{
		const CarromPuck* puck = &state->pucks[i];
		if (b2Body_IsEnabled(puck->bodyId))
		{
			snapshot.enabledPucks[snapshot.numOfEnabledPucks].index = puck->index;
			snapshot.enabledPucks[snapshot.numOfEnabledPucks].position = b2Body_GetPosition(puck->bodyId);
			snapshot.numOfEnabledPucks++;
		}
	}

	return snapshot;
}

void CarromGameState_ApplySnapshot(CarromGameState* state, const CarromSnapshot* snapshot, const bool recreate)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(snapshot != NULL);
	if (state == NULL || snapshot == NULL)
	{
		return;
	}

	if (recreate)
	{
		b2DestroyWorld(state->worldId);
		CarromGameState_CreateImpl(state,
		                           &state->worldDef,
		                           &state->pocketDef,
		                           &state->puckPhysicsDef,
		                           &state->strikerPhysicsDef,
		                           &state->strikerLimitDef,
		                           state->numOfPucks,
		                           state->pucksPositionDefs,
		                           state->numOfPockets,
		                           state->pocketsPositionDefs
			);
	}

	// disable striker
	b2Body_Disable(state->strikerBodyId);

	// disable all pucks
	for (int i = 0; i < state->numOfPucks; i++)
	{
		const CarromPuck* puck = &state->pucks[i];
		b2Body_Disable(puck->bodyId);
	}

	// enable pucks, set position
	for (int i = 0; i < snapshot->numOfEnabledPucks; i++)
	{
		const CarromPuckSnapshot* puckSnapshot = &snapshot->enabledPucks[i];
		const CarromPuck* puck = &state->pucks[puckSnapshot->index];
		b2Body_Enable(puck->bodyId);
		b2Body_SetTransform(puck->bodyId, puckSnapshot->position, b2Rot_identity);
	}
}

#include <stdio.h>

/**
 * NOTE: WE ASSUME THAT THERE ARE ONLY TWO TYPES OF MOVING OBJECTS:
 * 1. STRIKER
 * 2. PUCK
 * ANY OTHER OBJECTS SHOULD NOT BE DYNAMIC
 *
 * @param state Game state
 * @param output Output frame
 */
void CarromGameState_DumpSingleFrame(const CarromGameState* state, CarromFrame* output)
{
	bool strikerHitPocket = false;
	bool pucksHitPocket[MAX_PUCK_CAPACITY] = {0};
	bool strikerHitOtherObject = false;
	bool pucksHitOtherObject[MAX_PUCK_CAPACITY] = {0};

	// sensor events
	const b2SensorEvents sensorEvents = b2World_GetSensorEvents(state->worldId);
	if (sensorEvents.beginCount > 0)
	{
		for (int i = 0; i < sensorEvents.beginCount; i++)
		{
			const b2SensorBeginTouchEvent* sensorEvent = &sensorEvents.beginEvents[i];
			const b2ShapeId sensorShapeId = sensorEvent->sensorShapeId;
			const b2BodyId sensorBodyId = b2Shape_GetBody(sensorShapeId);

			if (!B2_ID_EQUALS(sensorBodyId, state->wallBodyId))
			{
				continue;
			}

			const b2ShapeId visitorId = sensorEvent->visitorShapeId;
			const b2BodyId bodyId = b2Shape_GetBody(visitorId);

			if (B2_ID_EQUALS(bodyId, state->strikerBodyId))
			{
				// striker hits the sensor, foul
				strikerHitPocket = true;

				// disable striker
				b2Body_Disable(state->strikerBodyId);
			}
			else
			{
				// puck hits the sensor
				const int32_t puckIndex = (int32_t)(intptr_t)b2Body_GetUserData(bodyId);
				MACARON_ASSERT(puckIndex >= 0 && puckIndex < state->numOfPucks);
				pucksHitPocket[puckIndex] = true;

				// disable puck
				b2Body_Disable(state->pucks[puckIndex].bodyId);
			}
		}
	}

	// dump contacts
	const b2ContactEvents contactEvents = b2World_GetContactEvents(state->worldId);
	if (contactEvents.beginCount > 0)
	{
		for (int i = 0; i < contactEvents.beginCount; i++)
		{
			const b2ContactBeginTouchEvent* contactEvent = &contactEvents.beginEvents[i];
			const b2ShapeId shapeIdA = contactEvent->shapeIdA;
			const b2ShapeId shapeIdB = contactEvent->shapeIdB;

			const b2BodyId bodyIdA = b2Shape_GetBody(shapeIdA);
			const b2BodyId bodyIdB = b2Shape_GetBody(shapeIdB);

			b2BodyId strikerBodyId = b2_nullBodyId;
			b2BodyId puckBodyId = b2_nullBodyId;

			if (B2_ID_EQUALS(bodyIdA, state->strikerBodyId))
			{
				strikerBodyId = bodyIdA;
			}
			else if (!B2_ID_EQUALS(bodyIdA, state->wallBodyId))
			{
				puckBodyId = bodyIdA;
			}

			if (B2_ID_EQUALS(bodyIdB, state->strikerBodyId))
			{
				strikerBodyId = bodyIdB;
			}
			else if (!B2_ID_EQUALS(bodyIdB, state->wallBodyId))
			{
				puckBodyId = bodyIdB;
			}

			if (B2_IS_NON_NULL(strikerBodyId))
			{
				strikerHitOtherObject = true;
			}
			if (B2_IS_NON_NULL(puckBodyId))
			{
				const int32_t puckIndex = (int32_t)(intptr_t)b2Body_GetUserData(puckBodyId);
				MACARON_ASSERT(puckIndex >= 0 && puckIndex < state->numOfPucks);
				pucksHitOtherObject[puckIndex] = true;
			}
		}
	}

	// dump movements
	const b2BodyEvents bodyEvents = b2World_GetBodyEvents(state->worldId);
	if (bodyEvents.moveCount > 0)
	{
		for (int i = 0; i < bodyEvents.moveCount; i++)
		{
			const b2BodyMoveEvent* bodyMoveEvent = &bodyEvents.moveEvents[i];
			CarromObjectMovement* movement = &output->movements[output->movementCount];
			movement->position = bodyMoveEvent->transform.p;

			if (B2_ID_EQUALS(bodyMoveEvent->bodyId, state->strikerBodyId))
			{
				// striker moves
				movement->type = CarromObjectType_Striker;
				movement->hitPocket = strikerHitPocket;
				movement->hitObject = strikerHitOtherObject;
			}
			else
			{
				// puck moves
				const int32_t puckIndex = (int32_t)(intptr_t)b2Body_GetUserData(bodyMoveEvent->bodyId);
				movement->type = CarromObjectType_Puck;
				movement->index = puckIndex;
				movement->hitPocket = pucksHitPocket[puckIndex];
				movement->hitObject = pucksHitOtherObject[puckIndex];
			}

			output->movementCount++;
		}
	}
}

CarromEvalResult CarromGameState_Eval(const CarromGameState* state, const int maxSteps)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(maxSteps <= MAX_FRAME_CAPACITY);
	MACARON_ASSERT(b2World_IsValid(state->worldId));

	CarromEvalResult result = {0};

	const int caps = maxSteps == 0 ? MAX_FRAME_CAPACITY : maxSteps;

	while (result.frameCount < caps)
	{
		b2World_Step(state->worldId, state->worldDef.frameDuration, state->worldDef.subStep);

		// dump frame
		CarromFrame* frame = &result.frames[result.frameCount];
		frame->index = result.frameCount;

		CarromGameState_DumpSingleFrame(state, frame);

		result.frameCount++;

		if (!CarromGameState_HasMovement(state))
		{
			// disable striker
			b2Body_Disable(state->strikerBodyId);

			break;
		}
	}

	return result;
}

void CarromGameState_Destroy(CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return;
	}

	b2DestroyWorld(state->worldId);
	state->worldId = b2_nullWorldId;
}
