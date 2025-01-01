#pragma once

#include "base.h"
#include "types.h"

MACARON_API CarromGameDef CarromGameDefLoadFromToml(const char* path);

MACARON_API CarromGameState CarromGameState_New(const CarromGameDef *def);

// Pucks

MACARON_API void CarromPuck_SetPosition(const CarromPuck *puck, b2Vec2 pos);

MACARON_API b2Vec2 CarromPuck_GetPosition(const CarromPuck *puck);

MACARON_API bool CarromPuck_IsEnable(const CarromPuck *puck);

MACARON_API void CarromPuck_SetIsEnable(CarromPuck *puck, bool enable);

MACARON_API void CarromPuck_Reset(CarromPuck *puck);
