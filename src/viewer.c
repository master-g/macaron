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

	viewer.strikerEnabled = true;
	viewer.strikerPosition = b2Body_GetPosition(state->strikerBodyId);

	for (int i = 0; i < state->numOfPucks; i++)
	{
		const CarromPuck* puck = &state->pucks[i];
		viewer.puckEnable[i] = b2Body_IsEnabled(puck->bodyId);
		viewer.puckPosition[i] = b2Body_GetPosition(puck->bodyId);
		viewer.puckColor[i] = puck->color;
	}
	viewer.numOfPucks = state->numOfPucks;

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

	for (int i = 0; i < frame->movementCount; i++)
	{
		const CarromObjectMovement* movement = &frame->movements[i];
		if (movement->type == CarromObjectType_Striker)
		{
			viewer->strikerPosition = movement->position;
			if (movement->hitPocket)
			{
				viewer->strikerEnabled = false;
			}
		}
		else if (movement->type == CarromObjectType_Puck)
		{
			viewer->puckPosition[movement->index] = movement->position;
			if (movement->hitPocket)
			{
				viewer->puckEnable[movement->index] = false;
			}
		}
	}
}
