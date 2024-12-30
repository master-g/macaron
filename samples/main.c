#include <stdio.h>
#include <sys/time.h>

#include "macaron/macaron.h"

#define FRAME_DURATION (1/60.0)
#define WORKER_COUNT 4
#define SUB_STEPS 4

#define NUM_OF_PUCKS 19

typedef enum PuckColor
{
    PuckColor_White,
    PuckColor_Black,
    PuckColor_Red,
} PuckColor;

#define PUCK_R 0.16f

typedef struct
{
    b2BodyId bodyId;
    PuckColor color;
} Puck;

Puck Puck_Spawn(const b2WorldId worldId, const PuckColor color, const b2Vec2 position, void* userData)
{
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.isBullet = true;
    bodyDef.linearDamping = 1.8f;
    bodyDef.angularDamping = 2.0f;
    bodyDef.position = position;
    bodyDef.userData = userData;

    const b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.friction = 0.0f;
    shapeDef.restitution = 1.0f;
    shapeDef.density = 0.45f;

    const Puck puck = { bodyId, color };

    b2Body_GetUserData(bodyId);

    return puck;
}

void Puck_DeSpawn(Puck *puck)
{
    b2DestroyBody(puck->bodyId);
    puck->bodyId = b2_nullBodyId;
}

typedef struct
{
    b2WorldDef worldDef;
    b2WorldId worldId;
    Puck pucks[NUM_OF_PUCKS];
} CarromState;

CarromState Carrom_CreateState(const b2WorldId worldId)
{
    CarromState state = { 0 };

    for (int i = 0; i < NUM_OF_PUCKS; i++)
    {
        void* index = (void*)(intptr_t)i;
        state.pucks[i] = Puck_Spawn(worldId, PuckColor_White, b2Vec2_zero, index);
    }

    return state;
}

int main() {
    const int steps = 10000;

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = b2Vec2_zero;
    worldDef.enableContinous = true;
    worldDef.workerCount = WORKER_COUNT;
    worldDef.enableSleep = false;

    const b2WorldId worldId = b2CreateWorld(&worldDef);

    const time_t now = time(NULL);

    for (int i = 0; i < steps; i++)
    {
        b2World_Step(worldId, FRAME_DURATION, SUB_STEPS);
    }

    const time_t elapsed_ms = (time(NULL) - now) * 1000;

    printf("Elapsed time: %ld ms\n", elapsed_ms);

    b2DestroyWorld(worldId);

  return 0;
 }