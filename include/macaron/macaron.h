#pragma once

#include "base.h"
#include "types.h"

// Game def

MACARON_API CarromGameDef CarromGameDefLoadFromToml(const char* path);

// Pucks

MACARON_API void CarromPuck_SetPosition(const CarromPuck* puck, b2Vec2 pos);

MACARON_API b2Vec2 CarromPuck_GetPosition(const CarromPuck* puck);

MACARON_API bool CarromPuck_IsEnable(const CarromPuck* puck);

MACARON_API void CarromPuck_SetIsEnable(CarromPuck* puck, bool enable);

MACARON_API void CarromPuck_Reset(CarromPuck* puck);

// Game state

/**
 * @defgroup game_state Game state
 * @brief Create new game state
 *
 * @param def game definition
 *
 * @return game state instance
 */
MACARON_API CarromGameState CarromGameState_New(const CarromGameDef* def);

/**
 * @brief Step game state, make updates
 *
 * @param state game state
 */
MACARON_API void CarromGameState_Step(const CarromGameState* state);

/**
 * @brief Place puck to the desired position
 *
 * @param state game state
 * @param puck target puck
 * @param step step distance
 *
 * @return nearest puck data
 */
MACARON_API b2Vec2 CarromGameState_PlacePuck(const CarromGameState* state, const CarromPuck* puck, float step);

/**
 * @brief Place pucks to the center of the table
 *
 * since there might be some pucks in or near the center of the table,
 * the final position of the puck might be different from the center
 *
 * @param state game state
 * @param index puck index
 *
 * @return final position of the puck
 */
MACARON_API b2Vec2 CarromGameState_PlacePuckToCenter(const CarromGameState* state, int index);

/**
 * @brief Place striker to the desired position
 *
 * since there might be some pucks blocking the way,
 * the final position of the striker might be different from the desired position
 *
 * @param state game state
 * @param tablePos position on the table
 * @param x desire x translation
 *
 * @return final position of the striker
 */
MACARON_API b2Vec2 CarromGameState_PlaceStriker(const CarromGameState* state, CarromTablePosition tablePos, float x);

/**
 * @brief Check if the striker is overlapping with other pucks
 *
 * @param state game state
 * @param pos position of the striker
 *
 * @return true if overlapping
 */
MACARON_API bool CarromGameState_IsStrikerOverlapping(const CarromGameState* state, b2Vec2 pos);

/**
 * @brief Apply force to the strike
 *
 * @param state game state
 * @param tablePos position on the table
 * @param impulse force and direction
 */
MACARON_API void CarromGameState_Strike(const CarromGameState* state, CarromTablePosition tablePos, b2Vec2 impulse);

/**
 * @brief Destroy game state and free memory
 *
 * @param state game state
 */
MACARON_API void CarromGameState_Destroy(CarromGameState* state);
