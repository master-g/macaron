#include "core.h"

#include <macaron/macaron.h>

void CarromPuck_SetPosition(const CarromPuck* puck, const b2Vec2 pos)
{
	MACARON_ASSERT(puck != NULL);
	b2Body_SetTransform(puck->bodyId, pos, b2Rot_identity);
}

b2Vec2 CarromPuck_GetPosition(const CarromPuck* puck)
{
	MACARON_ASSERT(puck != NULL);
	return b2Body_GetPosition(puck->bodyId);
}

bool CarromPuck_IsEnable(const CarromPuck* puck)
{
	MACARON_ASSERT(puck != NULL);
	return b2Body_IsEnabled(puck->bodyId);
}

void CarromPuck_SetIsEnable(CarromPuck* puck, bool enable)
{
	MACARON_ASSERT(puck != NULL);
	enable ? b2Body_Enable(puck->bodyId) : b2Body_Disable(puck->bodyId);
}

void CarromPuck_Reset(CarromPuck* puck)
{
	MACARON_ASSERT(puck != NULL);
	CarromPuck_SetPosition(puck, puck->originPos);
	CarromPuck_SetIsEnable(puck, true);
}
