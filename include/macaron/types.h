#pragma once

#include "base.h"

#define MAX_PUCK_CAPACITY 19
#define MAX_POCKET_CAPACITY 4

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
	// disable sleep, default is false
	bool disableSleep;
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

// Player position
typedef enum CarromTablePosition
{
	CarromTablePosition_Top,
	CarromTablePosition_Bottom,
} CarromTablePosition;

// Puck position def
typedef struct CarromPuckPositionDef
{
	// color for pucks
	CarromPuckColor color;
	// position
	b2Vec2 position;
} CarromPuckPositionDef;

// Pocket position def
typedef struct CarromPocketPositionDef
{
	// position
	b2Vec2 position;
} CarromPocketPositionDef;

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
	// pocket count
	int32_t numOfPockets;
	// pocket positions
	CarromPocketPositionDef pocketsPositions[MAX_POCKET_CAPACITY];
	// pucks count
	int32_t numOfPucks;
	// pucks positions
	CarromPuckPositionDef pucksPositions[MAX_PUCK_CAPACITY];
} CarromGameDef;

MACARON_API CarromGameDef CarromDefaultGameDef(void);

MACARON_API void CarromGameDef_PlacePucks(CarromGameDef* def);

MACARON_API void CarromGameDef_PlacePockets(CarromGameDef* state);

// Puck instance
typedef struct CarromPuck
{
	// index, [0, NUM_OF_PUCKS)
	int32_t index;
	// color of this puck instance
	CarromPuckColor color;
	// body id in Box2D
	b2BodyId bodyId;
} CarromPuck;

// Striker instance
typedef struct CarromStriker
{
	// Body id in Box2D
	b2BodyId bodyId;
} CarromStriker;

// Game instance
typedef struct CarromGameState
{
	// world def
	CarromWorldDef worldDef;
	// puck physics def
	CarromPuckPhysicsDef puckPhysicsDef;
	// pocket def
	CarromPocketDef pocketDef;
	// striker physics def
	CarromPuckPhysicsDef strikerPhysicsDef;
	// striker limit def
	CarromStrikerLimitDef strikerLimitDef;

	// world id in Box2D
	b2WorldId worldId;

	// wall bodyId
	b2BodyId wallBodyId;

	// number of pockets
	int32_t numOfPockets;

	// pocket position definitions
	CarromPocketPositionDef pocketsPositionDefs[MAX_POCKET_CAPACITY];

	// pockets
	b2ShapeId pockets[MAX_POCKET_CAPACITY];

	// number of pucks
	int32_t numOfPucks;

	// pucks
	CarromPuck pucks[MAX_PUCK_CAPACITY];

	// puck position definitions
	CarromPuckPositionDef pucksPositionDefs[MAX_PUCK_CAPACITY];

	// striker
	b2BodyId strikerBodyId;

} CarromGameState;

typedef struct CarromPuckSnapshot
{
	// object index
	int32_t index;
	// position
	b2Vec2 position;
} CarromPuckSnapshot;

// Game object position
typedef struct CarromStateSnapshot
{
	// number of enabled pucks
	int numOfEnabledPucks;
	// enabled pucks
	CarromPuckSnapshot enabledPucks[MAX_PUCK_CAPACITY];
} CarromStateSnapshot;
