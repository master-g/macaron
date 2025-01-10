#pragma once

#include "types.h"

typedef struct CarromEvalResultViewer
{
	bool enables[NUM_OF_OBJECTS];
	b2Vec2 positions[NUM_OF_OBJECTS];
	bool rest[NUM_OF_OBJECTS];

	float radius[CarromObjectType_Count];

	b2Vec2 pocketsPosition[MAX_POCKET_CAPACITY];

	float worldWidth;
	float worldHeight;

	float strikerLimitWidth;
	float strikerLimitOffCenter;

} CarromEvalResultViewer;

MACARON_API CarromEvalResultViewer CarromEvalResultViewerCreate(const CarromGameState* state);

MACARON_API void CarromEvalResultViewer_Update(CarromEvalResultViewer* viewer, const CarromFrame* frame);
