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

MACARON_API CarromWorldDef CarromDefaultWorldDef(void);

// Puck physics def
typedef struct CarromPuckPhysicsDef
{
	// radius
	float radius;
	// gap
	float gap;
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

MACARON_API CarromPuckPhysicsDef CarromDefaultPuckPhysicsDef(void);

MACARON_API CarromPuckPhysicsDef CarromDefaultStrikerPhysicsDef(void);

// Pocket def
typedef struct CarromPocketDef
{
	float radius;
} CarromPocketDef;

MACARON_API CarromPocketDef CarromDefaultPocketDef(void);

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
} CarromPuckColor;

// Puck position def
typedef struct CarromPuckPositionDef
{
	// object index
	int32_t index;
	// color for pucks
	CarromPuckColor color;
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

MACARON_API CarromStrikerLimitDef CarromDefaultStrikerLimitDef(void);

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

MACARON_API CarromGameDef CarromDefaultGameDef(void);

MACARON_API void CarromGameDef_PlacePucks(CarromGameDef *def);

// Puck instance
typedef struct CarromPuck
{
	// index, from 1 up to NUM_OF_PUCKS
	int32_t index;
	// color of this puck instance
	CarromPuckColor color;
	// body id in Box2D
	b2BodyId bodyId;
} CarromPuck;
