#include <stdio.h>
#include <time.h>

#include "macaron/macaron.h"

int main() {
	const CarromGameDef gameDef = CarromGameDefLoadFromToml("samples/config/carrom_config_example.toml");
	for (int i = 0; i < gameDef.numOfPucks; i++)
	{
		const CarromPuckPositionDef posDef = gameDef.pucksPositions[i];
		printf("idx: %d, color: %d, pos: (%.3f, %.3f)\n", posDef.index, posDef.color, posDef.position.x, posDef.position.y);
	}
	CarromGameState state = CarromGameState_New(&gameDef);

	b2World_Explode(state.worldId, b2Vec2_zero, 1.0f, 50.0f);

	struct timespec start, end;
	timespec_get(&start, TIME_UTC);

	const int steps = 10000;
    for (int i = 0; i < steps; i++)
    {
    	CarromGameState_Step(&state);
    	const b2BodyEvents events = b2World_GetBodyEvents(state.worldId);
    	if (events.moveCount == 0)
    	{
    		printf("no more moves, break early, steps: %d\n", i);
    		break;
    	}
    }

	timespec_get(&end, TIME_UTC);

	double elapsed = (end.tv_sec - start.tv_sec);
	elapsed += (end.tv_nsec - start.tv_nsec) / 1e9;

	for (int i = 0; i < state.numOfPucks; i++)
	{
		const CarromPuck *puck = &state.pucks[i];
		if (CarromPuck_IsEnable(puck))
		{
			const b2Vec2 pos = CarromPuck_GetPosition(puck);
			printf("puck %d pos: (%.3f, %.3f)\n", puck->index, pos.x, pos.y);
		}
	}

	CarromGameState_Destroy(&state);

    printf("Elapsed time: %.9f s\n", elapsed);

	return 0;
 }