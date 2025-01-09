#include "core.h"

#include <macaron/viewer.h>

CarromEvalResultViewer CarromEvalResultViewerCreate(const CarromGameState* state)
{
	CarromEvalResultViewer viewer = {0};
	MACARON_ASSERT(state != NULL);
	if (state == NULL)
	{
		return viewer;
	}

	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		const CarromObject* obj = &state->objects[i];
		if (B2_ID_EQUALS(obj->bodyId, b2_nullBodyId))
		{
			viewer.enables[i] = false;
			viewer.positions[i] = b2Vec2_zero;
		}
		else
		{
			viewer.enables[i] = b2Body_IsEnabled(obj->bodyId);
			viewer.positions[i] = b2Body_GetPosition(obj->bodyId);
		}
	}

	for (int i = 0; i < state->numOfPockets; i++)
	{
		viewer.pocketsPosition[i] = state->pocketsPositionDefs[i].position;
	}
	viewer.numOfPockets = state->numOfPockets;

	viewer.worldWidth = state->worldDef.width;
	viewer.worldHeight = state->worldDef.height;
	viewer.strikerLimitWidth = state->strikerLimitDef.width;
	viewer.strikerLimitOffCenter = state->strikerLimitDef.centerOffset;

	viewer.radius[CarromObjectType_Puck] = state->puckPhysicsDef.radius;
	viewer.radius[CarromObjectType_Striker] = state->strikerPhysicsDef.radius;
	viewer.radius[CarromObjectType_Pocket] = state->pocketDef.radius;

	return viewer;
}

void CarromEvalResultViewer_Update(CarromEvalResultViewer* viewer, const CarromFrame* frame)
{
	MACARON_ASSERT(viewer != NULL);
	MACARON_ASSERT(frame != NULL);
	if (viewer == NULL || frame == NULL)
	{
		return;
	}

	for (int i = 0; i < NUM_OF_OBJECTS; i++)
	{
		const CarromObjectSnapshot* snapshot = &frame->snapshots[i];
		if (MACARON_IS_VALID_OBJ_IDX(snapshot->index))
		{
			viewer->enables[snapshot->index] = snapshot->enable;
			viewer->positions[snapshot->index] = snapshot->position;
		}
	}
}
