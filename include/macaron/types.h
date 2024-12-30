#pragma once

#include "base.h"

#define NUM_OF_PUCKS 19

// Box2D world def
typedef struct CarromWorldDef
{
	// world size
	float width;
	// world height
	float height;
	// Box2D worker count, default is 4
	int32_t workerCount;
	// Box2D world step sub steps, default is 4
	int32_t subStep;
	// enable sleep, default is false
	bool enableSleep;
	// frame duration, default is 1/60.0
	float frameDuration;
} CarromWorldDef;

// Puck physics def
typedef struct CarromPuckPhysicsDef
{
	// radius
	float radius;
	// body linear damping
	float bodyLinearDamping;
	// body angular damping
	float bodyAngularDamping;
	// shape friction
	float shapeFriction;
	// shape restitution
	float shapeRestitution;
	// shape density
	float shapeDensity;
} CarromPuckPhysicsDef;

// Pocket def
typedef struct CarromPocketDef
{
	float radius;
} CarromPocketDef;

// Object types
typedef enum CarromObjectType
{
	CarromObjectType_Puck,
	CarromObjectType_Striker,
	CarromObjectType_Pocket,
} CarromObjectType;

// Puck color
typedef enum CarromPuckColor
{
	PuckColor_White,
	PuckColor_Black,
	PuckColor_Red,
} PuckColor;

// Puck position def
typedef struct CarromPuckPositionDef
{
	// object index
	int32_t index;
	// color for pucks
	PuckColor color;
	// position
	b2Vec2 position;
} CarromPuckPositionDef;

// Striker limitation def
typedef struct CarromStrikerLimitDef
{
	// width
	float width;
	// vertical offset from center
	float centerOffset;
	// maximum force when strike
	float maxForce;
} CarromStrikerLimitDef;

// Game def
typedef struct CarromGameDef
{
	// world def
	CarromWorldDef worldDef;
	// pucks physics def
	CarromPuckPhysicsDef puckPhysicsDef;
	// striker physics def
	CarromPuckPhysicsDef strikerPhysicsDef;
	// pocket def
	CarromPocketDef pocketDef;
	// striker def
	CarromStrikerLimitDef strikerLimitDef;
	// pucks count
	int32_t numOfPucks;
	// pucks positions
	CarromPuckPositionDef pucksPositions[NUM_OF_PUCKS];
} CarromGameDef;
