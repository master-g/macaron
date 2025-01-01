#include "core.h"

#include <macaron/macaron.h>

void CarromPuck_SetPosition(const CarromPuck* puck, const b2Vec2 pos)
{
	MACARON_ASSERT(puck != NULL);
	b2Body_GetWorldPoint(puck->bodyId, pos);
}

b2Vec2 CarromPuck_GetPosition(const CarromPuck* puck)
{
	MACARON_ASSERT(puck != NULL);
	return b2Body_GetPosition(puck->bodyId);
}
