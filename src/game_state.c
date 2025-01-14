#include <macaron/types.h>
#include <macaron/macaron.h>

#include "core.h"

const int32_t sPuckIndexes[PUCK_IDX_COUNT] = {
	IDX_PUCK_BLACK_START,
	IDX_PUCK_BLACK_START + 1,
	IDX_PUCK_BLACK_START + 2,
	IDX_PUCK_BLACK_START + 3,
	IDX_PUCK_BLACK_START + 4,
	IDX_PUCK_BLACK_START + 5,
	IDX_PUCK_BLACK_START + 6,
	IDX_PUCK_BLACK_START + 7,
	IDX_PUCK_BLACK_START + 8,

	IDX_PUCK_WHITE_START,
	IDX_PUCK_WHITE_START + 1,
	IDX_PUCK_WHITE_START + 2,
	IDX_PUCK_WHITE_START + 3,
	IDX_PUCK_WHITE_START + 4,
	IDX_PUCK_WHITE_START + 5,
	IDX_PUCK_WHITE_START + 6,
	IDX_PUCK_WHITE_START + 7,
	IDX_PUCK_WHITE_START + 8,

	IDX_PUCK_RED,
};

CarromObject CarromPuck_New(const b2WorldId worldId, const CarromObjectPhysicsDef* physicsDef)
{
	CarromObject puck = {0};
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
                                const CarromObjectPhysicsDef* puckPhysicsDef,
                                const CarromObjectPhysicsDef* strikerPhysicsDef,
                                const CarromStrikerLimitDef* strikerLimitDef)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(def != NULL);
	MACARON_ASSERT(pocketDef != NULL);
	MACARON_ASSERT(puckPhysicsDef != NULL);
	MACARON_ASSERT(strikerPhysicsDef != NULL);
	MACARON_ASSERT(strikerLimitDef != NULL);

	if (state == NULL
	    || def == NULL
	    || pocketDef == NULL
	    || puckPhysicsDef == NULL
	    || strikerPhysicsDef == NULL
	    || strikerLimitDef == NULL
	)
	{
		return;
	}

	if (b2World_IsValid(state->worldId))
	{
		b2DestroyWorld(state->worldId);
		state->worldId = b2_nullWorldId;
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

		b2Body_SetUserData(state->wallBodyId, (void*)(intptr_t)IDX_WALL);
	}

	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		state->objects[i].bodyId = b2_nullBodyId;
	}

	// pucks
	CarromObject nullObj = {0};
	nullObj.index = -1;
	nullObj.bodyId = b2_nullBodyId;

	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		if (MACARON_IS_VALID_PUCK_IDX(i))
		{
			CarromObject puck = CarromPuck_New(state->worldId, puckPhysicsDef);
			puck.index = i;
			state->objects[i] = puck;

			b2Body_SetTransform(puck.bodyId, b2Vec2_zero, b2Rot_identity);
			b2Body_SetUserData(puck.bodyId, (void*)(intptr_t)puck.index);
			b2Body_Disable(puck.bodyId);
		}
		else
		{
			state->objects[i] = nullObj;
		}
	}

	// pockets
	{
		const float left = -def->width / 2 + pocketDef->cornerOffsetX;
		const float right = def->width / 2 - pocketDef->cornerOffsetX;
		const float top = def->height / 2 - pocketDef->cornerOffsetY;
		const float bottom = -def->height / 2 + pocketDef->cornerOffsetY;

		const b2Vec2 pocketsPositions[MAX_POCKET_CAPACITY] = {
			{left, top},
			{right, top},
			{right, bottom},
			{left, bottom},
		};

		for (int i = 0; i < MAX_POCKET_CAPACITY; i++)
		{
			const b2Circle circle = {pocketsPositions[i], pocketDef->radius};
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.isSensor = true;
			state->pockets[i] = b2CreateCircleShape(state->wallBodyId, &shapeDef, &circle);
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

		CarromObject striker = {0};
		striker.index = IDX_STRIKER;
		striker.bodyId = b2CreateBody(state->worldId, &strikerDef);

		b2Circle circle = {{0.0f, 0.0f}, strikerPhysicsDef->radius};
		b2CreateCircleShape(striker.bodyId, &shapeDef, &circle);

		b2Body_SetUserData(striker.bodyId, (void*)(intptr_t)IDX_STRIKER);

		state->objects[IDX_STRIKER] = striker;
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
	                           &def->strikerLimitDef);

	return state;
}

MACARON_API void CarromGameState_SetPuckPosition(const CarromGameState* state, const int size,
                                                 const CarromObjectPositionDef* positions)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(size > 0);
	MACARON_ASSERT(positions != NULL);

	if (state == NULL || size <= 0 || positions == NULL)
	{
		return;
	}

	for (int i = 0; i < size; i++)
	{
		const CarromObjectPositionDef posDef = positions[i];
		if (posDef.index == IDX_STRIKER)
		{
			CarromGameState_PlaceStrikerUnsafe(state, posDef.position);
			continue;
		}
		if (!MACARON_IS_VALID_PUCK_IDX(posDef.index))
		{
			continue;
		}

		const CarromObject* puck = &state->objects[posDef.index];
		b2Body_SetTransform(puck->bodyId, posDef.position, b2Rot_identity);
		b2Body_Enable(puck->bodyId);
	}
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

FindNearestPuckData CarromGameState_FindNearestPuck(const CarromGameState* state, const CarromObject* target,
                                                    const b2Vec2 pos)
{
	float minDistance = state->worldDef.width + state->worldDef.height;
	FindNearestPuckData result = {false, minDistance, b2Vec2_zero};

	for (int i = 0; i < PUCK_IDX_COUNT; i++)
	{
		const int idx = sPuckIndexes[i];
		const CarromObject* puck = &state->objects[idx];
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

b2Vec2 CarromGameState_GetPuckPosition(const CarromGameState* state, const int index)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(MACARON_IS_VALID_PUCK_IDX(index));
	if (state == NULL)
	{
		return b2Vec2_zero;
	}

	const CarromObject* puck = &state->objects[index];
	return b2Body_GetPosition(puck->bodyId);
}

b2Vec2 CarromGameState_GetStrikerPosition(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return b2Vec2_zero;
	}

	return b2Body_GetPosition(state->objects[IDX_STRIKER].bodyId);
}

b2Vec2 CarromGameState_PlacePuck(const CarromGameState* state, const CarromObject* puck, const float step)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(puck != NULL);
	if (state == NULL || puck == NULL)
	{
		return b2Vec2_zero;
	}

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
	MACARON_ASSERT(MACARON_IS_VALID_PUCK_IDX(index));
	if (state == NULL)
	{
		return b2Vec2_zero;
	}

	const CarromObject* puck = &state->objects[index];
	b2Body_SetTransform(puck->bodyId, pos, b2Rot_identity);

	const float step = state->puckPhysicsDef.radius / 200.0f;
	return CarromGameState_PlacePuck(state, puck, step);
}

void CarromGameState_PlacePuckToPosUnsafe(const CarromGameState* state, const int index, const b2Vec2 pos, bool enable)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(MACARON_IS_VALID_PUCK_IDX(index));

	const CarromObject* puck = &state->objects[index];
	b2Body_SetTransform(puck->bodyId, pos, b2Rot_identity);
	if (enable)
	{
		b2Body_Enable(puck->bodyId);
	} else
	{
		b2Body_Disable(puck->bodyId);
	}
}

b2Vec2 CarromGameState_PlacePuckToCenter(const CarromGameState* state, const int index)
{
	MACARON_ASSERT(MACARON_IS_VALID_PUCK_IDX(index));

	const CarromObject* puck = &state->objects[index];

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

	for (int i = 0; i < PUCK_IDX_COUNT; i++)
	{
		const int idx = sPuckIndexes[i];
		const CarromObject* puck = &state->objects[idx];

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

b2Vec2 CarromGameState_LimitStrikerPos(const CarromGameState* state, const CarromTablePosition tablePos, const b2Vec2 pos)
{
	b2Vec2 finalPos = pos;

	const float strikerLimitWidth = state->strikerLimitDef.width;
	const float centerOffset = state->strikerLimitDef.centerOffset;

	if (strikerLimitWidth == 0.0f || centerOffset == 0.0f)
	{
		return finalPos;
	}

	const float horizontalLeftX = -strikerLimitWidth / 2;
	const float horizontalRightX = strikerLimitWidth / 2;
	const float horizontalTopY = centerOffset;
	const float horizontalBottomY = -centerOffset;
	const float verticalTopY = strikerLimitWidth / 2;
	const float verticalBottomY = -strikerLimitWidth / 2;
	const float verticalLeftX = -centerOffset;
	const float verticalRightX = centerOffset;

	if (tablePos == CarromTablePosition_Bottom)
	{
		finalPos.y = horizontalBottomY;
	}
	else if (tablePos == CarromTablePosition_Left)
	{
		finalPos.x = verticalLeftX;
	}
	else if (tablePos == CarromTablePosition_Top)
	{
		finalPos.y = horizontalTopY;
	}
	else if (tablePos == CarromTablePosition_Right)
	{
		finalPos.x = verticalRightX;
	}

	if (tablePos == CarromTablePosition_Bottom || tablePos == CarromTablePosition_Top)
	{
		// limit x
		if (finalPos.x < horizontalLeftX)
		{
			finalPos.x = horizontalLeftX;
		}
		if (finalPos.x > horizontalRightX)
		{
			finalPos.x = horizontalRightX;
		}
	}
	else if (tablePos == CarromTablePosition_Left || tablePos == CarromTablePosition_Right)
	{
		// limit y
		if (finalPos.y < verticalBottomY)
		{
			finalPos.y = verticalBottomY;
		}
		if (finalPos.y > verticalTopY)
		{
			finalPos.y = verticalTopY;
		}
	}

	return finalPos;
}

void CarromGameState_EnableStriker(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return;
	}
	b2Body_Enable(state->objects[IDX_STRIKER].bodyId);
}

void CarromGameState_DisableStriker(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return;
	}
	b2Body_Disable(state->objects[IDX_STRIKER].bodyId);
}

bool CarromGameState_IsStrikerEnabled(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return false;
	}
	return b2Body_IsEnabled(state->objects[IDX_STRIKER].bodyId);
}

b2Vec2 CarromGameState_PlaceStriker(const CarromGameState* state, const CarromTablePosition tablePos, const b2Vec2 pos)
{
	const b2Vec2 finalPos = CarromGameState_LimitStrikerPos(state, tablePos, pos);

	const b2BodyId strikerBodyId = state->objects[IDX_STRIKER].bodyId;
	b2Body_SetTransform(strikerBodyId, finalPos, b2Rot_identity);

	const FindNearestPuckData nearestData = CarromGameState_FindStrikerNearestPuck(state, finalPos);
	if (!nearestData.found)
	{
		return finalPos;
	}

	const float puckR = state->puckPhysicsDef.radius;
	const float strikerR = state->strikerPhysicsDef.radius;

	if (nearestData.minDistance > puckR + strikerR)
	{
		return finalPos;
	}

	// got overlap
	const b2Circle circleStriker = {{0.0f, 0.0f}, strikerR};
	const b2Circle circlePuck = {{0.0f, 0.0f}, puckR};

	const b2Transform transformStriker = {finalPos, b2Rot_identity};
	const b2Transform transformPuck = {nearestData.nearestPos, b2Rot_identity};

	const b2Manifold m = b2CollideCircles(&circleStriker, transformStriker, &circlePuck, transformPuck);
	if (m.pointCount > 0)
	{
		const float pushOut = (strikerR + puckR) - nearestData.minDistance + 0.001f;
		const float normalX = m.normal.x;
		const float normalY = m.normal.y;

		float sepX = 0.0f;
		float sepY = 0.0f;
		float direction;

		if (tablePos == CarromTablePosition_Bottom || tablePos == CarromTablePosition_Top)
		{
			// should move horizontally
			if (normalX == 0.0f)
			{
				// force move to right
				direction = 1.0f;
			}
			else
			{
				direction = normalX > 0.0f ? -1.0f : 1.0f;
			}

			if (pushOut < 0.00001f || b2AbsFloat(normalX) < 0.00001f)
			{
				sepX = direction * 0.001f;
			}
			else
			{
				sepX = pushOut * normalX;
			}
		}
		else
		{
			// should move vertically
			if (normalY == 0.0f)
			{
				// force move to up
				direction = 1.0f;
			}
			else
			{
				direction = normalY > 0.0f ? -1.0f : 1.0f;
			}

			if (pushOut < 0.00001f || b2AbsFloat(normalY) < 0.00001f)
			{
				sepY = direction * 0.001f;
			}
			else
			{
				sepY = pushOut * normalY;
			}
		}

		b2Vec2 after;
		after.x = finalPos.x + sepX;
		after.y = finalPos.y + sepY;

		after = CarromGameState_LimitStrikerPos(state, tablePos, after);

		b2Body_SetTransform(strikerBodyId, after, b2Rot_identity);

		bool reversed = false;
		const float step = state->strikerPhysicsDef.radius / 100.0f;

		const float maxHorizontalRange = state->strikerLimitDef.width > 0.0f
			                                 ? state->strikerLimitDef.width / 2
			                                 : state->worldDef.width / 2;
		const float maxVerticalRange = state->strikerLimitDef.width > 0.0f
			                               ? state->strikerLimitDef.width / 2
			                               : state->worldDef.height / 2;

		while (true)
		{
			const b2Vec2 placePos = b2Body_GetPosition(strikerBodyId);
			const FindNearestPuckData data = CarromGameState_FindStrikerNearestPuck(state, placePos);
			if (!data.found || data.minDistance > strikerR + puckR)
			{
				return placePos;
			}

			b2Vec2 newPos = placePos;
			if (tablePos == CarromTablePosition_Bottom || tablePos == CarromTablePosition_Top)
			{
				newPos.x += step * direction;
				if (newPos.x < -maxHorizontalRange)
				{
					newPos.x = -maxHorizontalRange;
					if (reversed)
					{
						return newPos;
					}
					direction = -direction;
					reversed = true;
					newPos = after;
				}
				else if (newPos.x > maxHorizontalRange)
				{
					newPos.x = maxHorizontalRange;
					if (reversed)
					{
						return newPos;
					}
					direction = -direction;
					reversed = true;
					newPos = after;
				}
			}
			else if (tablePos == CarromTablePosition_Left || tablePos == CarromTablePosition_Right)
			{
				newPos.y += step * direction;
				if (newPos.y < -maxVerticalRange)
				{
					newPos.y = -maxVerticalRange;
					if (reversed)
					{
						return newPos;
					}
					direction = -direction;
					reversed = true;
					newPos = after;
				}
				else if (newPos.y > maxVerticalRange)
				{
					newPos.y = maxVerticalRange;
					if (reversed)
					{
						return newPos;
					}
					direction = -direction;
					reversed = true;
					newPos = after;
				}
			}

			b2Body_SetTransform(strikerBodyId, newPos, b2Rot_identity);
		}
	}

	return pos;
}

void CarromGameState_PlaceStrikerUnsafe(const CarromGameState* state, const b2Vec2 pos)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return;
	}

	const b2BodyId strikerBodyId = state->objects[IDX_STRIKER].bodyId;
	b2Body_SetTransform(strikerBodyId, pos, b2Rot_identity);
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

void CarromGameState_Strike(const CarromGameState* state, b2Vec2 impulse, const float maxForce)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return;
	}

	const b2BodyId strikerBodyId = state->objects[IDX_STRIKER].bodyId;

	const float force = b2Length(impulse);
	if (maxForce > 0.0f && force > maxForce)
	{
		const float scale = maxForce / force;
		impulse = b2MulSV(scale, impulse);
	}
	if (!b2Body_IsEnabled(strikerBodyId))
	{
		b2Body_Enable(strikerBodyId);
	}
	b2Body_ApplyForceToCenter(strikerBodyId, impulse, true);
}

void CarromGameState_ApplyVelocityToStriker(const CarromGameState* state, const b2Vec2 velocity)
{
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return;
	}

	const b2BodyId strikerBodyId = state->objects[IDX_STRIKER].bodyId;
	b2Body_Enable(strikerBodyId);
	b2Body_SetLinearVelocity(strikerBodyId, velocity);
}

CarromFrame CarromGameState_TakeSnapshot(const CarromGameState* state)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(b2World_IsValid(state->worldId));

	CarromFrame frame = {0};
	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		const CarromObject* obj = &state->objects[i];
		if (!B2_ID_EQUALS(b2_nullBodyId, obj->bodyId))
		{
			frame.snapshots[i].index = (int8_t) i;
			frame.snapshots[i].enable = b2Body_IsEnabled(obj->bodyId);
			frame.snapshots[i].position = b2Body_GetPosition(obj->bodyId);
		}
	}

	return frame;
}

void CarromGameState_ApplySnapshot(CarromGameState* state, const CarromFrame* frame, const bool recreate)
{
	MACARON_ASSERT(state != NULL);
	MACARON_ASSERT(frame != NULL);
	if (state == NULL || frame == NULL)
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
		                           &state->strikerLimitDef
			);
	}

	// disable all objects
	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		const b2BodyId bodyId = state->objects[i].bodyId;
		if (!B2_ID_EQUALS(bodyId, b2_nullBodyId))
		{
			b2Body_Disable(bodyId);
		}
	}

	// enable pucks, set position
	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		const CarromObjectSnapshot* objectSnapshot = &frame->snapshots[i];
		if (MACARON_IS_VALID_OBJ_IDX(objectSnapshot->index))
		{
			const CarromObject* object = &state->objects[objectSnapshot->index];
			if (objectSnapshot->enable)
			{
				b2Body_Enable(object->bodyId);
			}
			b2Body_SetTransform(object->bodyId, objectSnapshot->position, b2Rot_identity);
		}
	}
}

/**
 * NOTE: WE ASSUME THAT THERE ARE ONLY TWO TYPES OF MOVING OBJECTS:
 * 1. STRIKER
 * 2. PUCK
 * ANY OTHER OBJECTS SHOULD NOT BE DYNAMIC
 *
 * @param state Game state
 * @param frame Output frame
 * @param pucksHitPocket number of pucks hit pocket
 */
void CarromGameState_DumpSingleFrame(const CarromGameState* state, CarromFrame* frame, int8_t* pucksHitPocket)
{
	// striker body id
	const b2BodyId strikerBodyId = state->objects[IDX_STRIKER].bodyId;

	// dump all objects first
	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		CarromObjectSnapshot* snapshot = &frame->snapshots[i];
		const b2BodyId objectBodyId = state->objects[i].bodyId;
		if (!B2_ID_EQUALS(objectBodyId, b2_nullBodyId))
		{
			snapshot->index = (int8_t)i;
			snapshot->enable = b2Body_IsEnabled(objectBodyId);
			snapshot->position = b2Body_GetPosition(objectBodyId);
			snapshot->hitEvent = CarromHitEventType_None;

			// first frame, all objects are considered as moving
			snapshot->rest = frame->index != 0;
		}
		else
		{
			snapshot->index = -1;
		}
	}

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

			const int32_t objectIndex = (int32_t)(intptr_t)b2Body_GetUserData(bodyId);

			b2Body_Disable(bodyId);

			if (objectIndex == IDX_STRIKER)
			{
				frame->strikerHitPocket = true;
			}

			(*pucksHitPocket)++;
			frame->pucksHitPocket = *pucksHitPocket;

			frame->snapshots[objectIndex].hitPocketIndex = *pucksHitPocket;
			frame->snapshots[objectIndex].enable = false;
			frame->snapshots[objectIndex].hitPocket = true;
			frame->snapshots[objectIndex].hitEvent = CarromHitEventType_HitPocket;
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

			// case 1, striker hits the wall
			// case 2, puck hits the wall
			// case 3, striker hits the puck
			// case 4, puck hits the puck

			if (B2_ID_EQUALS(bodyIdA, state->wallBodyId) || B2_ID_EQUALS(bodyIdB, state->wallBodyId))
			{
				// wall
				if (B2_ID_EQUALS(bodyIdA, strikerBodyId) || B2_ID_EQUALS(bodyIdB, strikerBodyId))
				{
					// striker hits the wall
					if (frame->snapshots[IDX_STRIKER].hitEvent == CarromHitEventType_None)
					{
						frame->snapshots[IDX_STRIKER].hitEvent = CarromHitEventType_HitWall;
					}
				}
				else
				{
					// puck hits the wall
					b2BodyId puckBodyId;
					if (!B2_ID_EQUALS(bodyIdA, state->wallBodyId))
					{
						puckBodyId = bodyIdA;
					}
					else
					{
						puckBodyId = bodyIdB;
					}
					const int32_t puckIndex = (int32_t)(intptr_t)b2Body_GetUserData(puckBodyId);
					if (frame->snapshots[puckIndex].hitEvent == CarromHitEventType_None)
					{
						frame->snapshots[puckIndex].hitEvent = CarromHitEventType_HitWall;
					}
				}
			}
			else
			{
				if (B2_ID_EQUALS(bodyIdA, strikerBodyId) || B2_ID_EQUALS(bodyIdB, strikerBodyId))
				{
					// striker hits the puck
					b2BodyId puckBodyId;
					if (B2_ID_EQUALS(bodyIdA, strikerBodyId))
					{
						puckBodyId = bodyIdB;
					}
					else
					{
						puckBodyId = bodyIdA;
					}
					const int32_t otherIndex = (int32_t)(intptr_t)b2Body_GetUserData(puckBodyId);
					if (frame->snapshots[IDX_STRIKER].hitEvent == CarromHitEventType_None)
					{
						frame->snapshots[IDX_STRIKER].hitEvent = CarromHitEventType_HitPuck;
					}
					if (frame->snapshots[otherIndex].hitEvent == CarromHitEventType_None)
					{
						frame->snapshots[otherIndex].hitEvent = CarromHitEventType_HitPuck;
					}
				}
				else
				{
					// puck hits the puck
					const int32_t puckIndexA = (int32_t)(intptr_t)b2Body_GetUserData(bodyIdA);
					const int32_t puckIndexB = (int32_t)(intptr_t)b2Body_GetUserData(bodyIdB);
					if (frame->snapshots[puckIndexA].hitEvent == CarromHitEventType_None)
					{
						frame->snapshots[puckIndexA].hitEvent = CarromHitEventType_HitPuck;
					}
					if (frame->snapshots[puckIndexB].hitEvent == CarromHitEventType_None)
					{
						frame->snapshots[puckIndexB].hitEvent = CarromHitEventType_HitPuck;
					}
				}
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
			const int32_t idx = (int32_t)(intptr_t)b2Body_GetUserData(bodyMoveEvent->bodyId);
			if (idx >= 0 && idx < NUM_OF_OBJECTS)
			{
				frame->snapshots[idx].position = bodyMoveEvent->transform.p;
				frame->snapshots[idx].rest = false;
			}
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

	while (result.numFrames < caps)
	{
		b2World_Step(state->worldId, state->worldDef.frameDuration, state->worldDef.subStep);

		// dump frame
		CarromFrame* frame = &result.frames[result.numFrames];
		frame->index = result.numFrames;

		CarromGameState_DumpSingleFrame(state, frame, &frame->pucksHitPocket);

		result.strikerHitPocket |= frame->strikerHitPocket;
		result.pucksHitPocket = (int8_t)(result.pucksHitPocket + frame->pucksHitPocket);

		result.numFrames++;

		if (!CarromGameState_HasMovement(state))
		{
			// disable striker
			b2Body_Disable(state->objects[IDX_STRIKER].bodyId);

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

	if (b2World_IsValid(state->worldId))
	{
		b2DestroyWorld(state->worldId);
	}
	state->worldId = b2_nullWorldId;
}
