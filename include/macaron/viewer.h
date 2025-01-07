#pragma once

#include "types.h"

typedef struct CarromEvalResultViewer
{
	bool strikerEnabled;
	b2Vec2 strikerPosition;
	bool puckEnable[MAX_PUCK_CAPACITY];
	int numOfPucks;
	b2Vec2 puckPosition[MAX_PUCK_CAPACITY];
	CarromPuckColor puckColor[MAX_PUCK_CAPACITY];

	float radius[CarromObjectType_Count];

	int numOfPockets;
	b2Vec2 pocketsPosition[MAX_POCKET_CAPACITY];

	float worldWidth;
	float worldHeight;

	float strikerLimitWidth;
	float strikerLimitOffCenter;

} CarromEvalResultViewer;

MACARON_API CarromEvalResultViewer CarromEvalResultViewerCreate(const CarromGameState* state);

MACARON_API void CarromEvalResultViewer_Update(CarromEvalResultViewer* viewer, const CarromFrame* frame);
