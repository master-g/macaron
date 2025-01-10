#pragma once

#include "base.h"
#include "types.h"

// Game def

MACARON_API CarromGameDef CarromGameDefLoadFromToml(const char* path);

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
 * @brief Set puck position
 *
 * @param state game state
 * @param size number of pucks
 * @param positions puck positions
 */
MACARON_API void CarromGameState_SetPuckPosition(const CarromGameState* state, int size,
                                                 const CarromObjectPositionDef* positions);

/**
 * @brief Check if the game state has movement
 *
 * @param state game state
 *
 * @return true if current state has movement
 */
MACARON_API bool CarromGameState_HasMovement(const CarromGameState* state);

/**
 *@brief Check if the game state has sensor events
 *
 * @param state game state
 *
 * @return true if current state has sensor events
 */
MACARON_API bool CarromGameState_HasSensorEvents(const CarromGameState* state);

/**
 * @brief Step game state, make updates
 *
 * @param state game state
 */
MACARON_API void CarromGameState_Step(const CarromGameState* state);

/**
 * @brief Get puck position
 *
 * @param state game state
 * @param index puck index
 *
 * @return puck position
 */
MACARON_API b2Vec2 CarromGameState_GetPuckPosition(const CarromGameState* state, int index);

/**
 * @brief Get striker position
 *
 * @param state game state
 *
 * @return striker position
 */
MACARON_API b2Vec2 CarromGameState_GetStrikerPosition(const CarromGameState* state);

/**
 * @brief Place puck to the desired position
 *
 * this function will move the puck to the desired position first,
 *
 * and then perform collision detection to make sure the puck is not overlapping with other pucks
 *
 * so the final position of the puck might be different from the desired position
 *
 * @param state game state
 * @param index puck index
 * @param pos desired position
 *
 * @return final position of the puck
 */
MACARON_API b2Vec2 CarromGameState_PlacePuckToPos(const CarromGameState* state, int index, b2Vec2 pos);

/**
 * @brief Place puck to the desired position
 *
 * this function will just simply move the puck to the desired position, without any collision detection
 *
 * @param state game state
 * @param index puck index
 * @param pos desired position
 *
 */
MACARON_API void CarromGameState_PlacePuckToPosUnsafe(const CarromGameState* state, int index, b2Vec2 pos);

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

MACARON_API void CarromGameState_EnableStriker(const CarromGameState* state);

MACARON_API void CarromGameState_DisableStriker(const CarromGameState* state);

MACARON_API bool CarromGameState_IsStrikerEnabled(const CarromGameState* state);

/**
 * @brief Place striker to the desired position
 *
 * since there might be some pucks blocking the way,
 * the final position of the striker might be different from the desired position
 *
 * @param state game state
 * @param tablePos position on the table
 * @param pos desire striker translation
 *
 * @return final position of the striker
 */
MACARON_API b2Vec2 CarromGameState_PlaceStriker(const CarromGameState* state, CarromTablePosition tablePos, b2Vec2 pos);

/**
 * @brief Place striker to the desired position
 *
 * this function will just simply move the striker to the desired position, without any collision detection
 *
 * @param state game state
 * @param pos position of the striker
 *
 */
MACARON_API void CarromGameState_PlaceStrikerUnsafe(const CarromGameState* state, b2Vec2 pos);

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
 * @param impulse force and direction
 * @param maxForce maximum force allowed, 0 means no limit
 */
MACARON_API void CarromGameState_Strike(const CarromGameState* state, b2Vec2 impulse, float maxForce);

/**
 * @brief Apply velocity to the striker
 *
 * @param state game state
 * @param velocity velocity
 */
MACARON_API void CarromGameState_ApplyVelocityToStriker(const CarromGameState* state, b2Vec2 velocity);

/**
 * @brief Take snapshot of the game state
 *
 * @param state game state
 *
 * @return snapshot of the game state
 */
MACARON_API CarromFrame CarromGameState_TakeSnapshot(const CarromGameState* state);

/**
 * @brief Apply snapshot to the game state
 *
 * @param state game state
 * @param frame snapshot of the game state
 * @param recreate recreate the game state
 */
MACARON_API void CarromGameState_ApplySnapshot(CarromGameState* state, const CarromFrame* frame, bool recreate);

/**
 * @brief Let the game state steps until no more movements
 *
 * @param state game state
 * @param maxSteps maximum steps allowed, should be smaller than MAX_FRAME_CAPACITY
 *
 * @return evaluation result
 */
MACARON_API CarromEvalResult CarromGameState_Eval(const CarromGameState* state, int maxSteps);

/**
 * @brief Destroy game state and free memory
 *
 * @param state game state
 */
MACARON_API void CarromGameState_Destroy(CarromGameState* state);
