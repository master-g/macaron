#pragma once

#include "base.h"

// 0: striker, 1-9: black pucks, 11-19: white pucks, 21: red puck
#define NUM_OF_OBJECTS 22
#define MAX_POCKET_CAPACITY 4
#define MAX_FRAME_CAPACITY 512

#define IDX_WALL (-1)

#define IDX_STRIKER 0
#define IDX_PUCK_BLACK_START 1
#define IDX_PUCK_BLACK_END 10
#define IDX_PUCK_WHITE_START 11
#define IDX_PUCK_WHITE_END 20
#define IDX_PUCK_RED 21

#define PUCK_IDX_COUNT 19

extern const int32_t sPuckIndexes[PUCK_IDX_COUNT];

#define MACARON_IS_VALID_PUCK_IDX(i) \
	((i) >= IDX_PUCK_BLACK_START && (i) < IDX_PUCK_BLACK_END || \
	 (i) >= IDX_PUCK_WHITE_START && (i) < IDX_PUCK_WHITE_END || \
	 (i) == IDX_PUCK_RED)

#define MACARON_IS_VALID_OBJ_IDX(i) \
	((i) == IDX_STRIKER || MACARON_IS_VALID_PUCK_IDX(i))

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

// Carrom object physics def
typedef struct CarromObjectPhysicsDef
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

} CarromObjectPhysicsDef;

MACARON_API CarromObjectPhysicsDef CarromDefaultPuckPhysicsDef(void);

MACARON_API CarromObjectPhysicsDef CarromDefaultStrikerPhysicsDef(void);

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

	CarromObjectType_Count,

} CarromObjectType;

#define MACARON_IDX_OBJECT_TYPE(idx) \
	((idx) == IDX_STRIKER ? CarromObjectType_Striker : \
	 (idx) >= IDX_PUCK_BLACK_START && (idx) < IDX_PUCK_BLACK_END ? CarromObjectType_Puck : \
	 (idx) >= IDX_PUCK_WHITE_START && (idx) < IDX_PUCK_WHITE_END ? CarromObjectType_Puck : \
	 (idx) == IDX_PUCK_RED ? CarromObjectType_Puck : CarromObjectType_Pocket)

// Puck color
typedef enum CarromPuckColor
{
	CarromPuckColor_White,
	CarromPuckColor_Black,
	CarromPuckColor_Red,

} CarromPuckColor;

#define MACARON_IDX_PUCK_COLOR(idx) \
	((idx) >= IDX_PUCK_BLACK_START && (idx) < IDX_PUCK_BLACK_END ? CarromPuckColor_Black : \
	 (idx) >= IDX_PUCK_WHITE_START && (idx) < IDX_PUCK_WHITE_END ? CarromPuckColor_White : \
	 (idx) == IDX_PUCK_RED ? CarromPuckColor_Red : CarromPuckColor_White)

// Player position
typedef enum CarromTablePosition
{
	CarromTablePosition_Bottom,
	CarromTablePosition_Left,
	CarromTablePosition_Top,
	CarromTablePosition_Right,

} CarromTablePosition;

// object position def
typedef struct CarromObjectPositionDef
{
	// object index, use for pucks
	int32_t index;
	// position
	b2Vec2 position;

} CarromObjectPositionDef;

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
	CarromObjectPhysicsDef puckPhysicsDef;
	// striker physics def
	CarromObjectPhysicsDef strikerPhysicsDef;
	// pocket def
	CarromPocketDef pocketDef;
	// striker def
	CarromStrikerLimitDef strikerLimitDef;
	// pocket count
	int32_t numOfPockets;
	// pocket positions
	CarromObjectPositionDef pocketsPositions[MAX_POCKET_CAPACITY];
	// pucks count
	int32_t numOfPucks;
	// pucks positions
	CarromObjectPositionDef pucksPositions[NUM_OF_OBJECTS];
} CarromGameDef;

MACARON_API CarromGameDef CarromDefaultGameDef(void);

MACARON_API void CarromGameDef_PlacePucks(CarromGameDef* def);

MACARON_API void CarromGameDef_PlacePockets(CarromGameDef* def);

// Object instance
typedef struct CarromObject
{
	// index
	int32_t index;
	// body id in Box2D
	b2BodyId bodyId;

} CarromObject;

// Game instance
typedef struct CarromGameState
{
	// world def
	CarromWorldDef worldDef;
	// puck physics def
	CarromObjectPhysicsDef puckPhysicsDef;
	// pocket def
	CarromPocketDef pocketDef;
	// striker physics def
	CarromObjectPhysicsDef strikerPhysicsDef;
	// striker limit def
	CarromStrikerLimitDef strikerLimitDef;

	// world id in Box2D
	b2WorldId worldId;

	// wall bodyId
	b2BodyId wallBodyId;

	// number of pockets
	int32_t numOfPockets;

	// pocket position definitions
	CarromObjectPositionDef pocketsPositionDefs[MAX_POCKET_CAPACITY];

	// pockets
	b2ShapeId pockets[MAX_POCKET_CAPACITY];

	// number of object position definitions
	int32_t numOfPosDefs;

	// object position definitions
	CarromObjectPositionDef objectPositionDefs[NUM_OF_OBJECTS];

	// objects
	CarromObject objects[NUM_OF_OBJECTS];

} CarromGameState;

typedef enum CarromHitEventType
{
	CarromHitEventType_None,
	CarromHitEventType_HitWall,
	CarromHitEventType_HitPuck,
	CarromHitEventType_HitPocket,

} CarromHitEventType;

// Carrom object snapshot
typedef struct CarromObjectSnapshot
{
	// object index
	int32_t index;
	// object position
	b2Vec2 position;
	// enable, object will be disabled if it hits the pocket
	bool enable;
	// hit event
	CarromHitEventType hitEvent;
	// just hit pocket
	bool hitPocket;
	// hit pocket index
	int32_t hitPocketIndex;

} CarromObjectSnapshot;

// Single frame of the carrom game
typedef struct CarromFrame
{
	// frame index
	int32_t index;
	// striker hits the pocket
	bool strikerHitPocket;
	// number of pucks hit the pocket
	int32_t pucksHitPocket;
	// object snapshots
	CarromObjectSnapshot snapshots[NUM_OF_OBJECTS];

} CarromFrame;

// Frames
typedef struct CarromEvalResult
{
	// striker hit the pocket
	bool strikerHitPocket;
	// number of pucks hit the pocket
	int32_t pucksHitPocket;
	// number of frames
	int32_t numFrames;
	// frames
	CarromFrame frames[MAX_FRAME_CAPACITY];

} CarromEvalResult;
