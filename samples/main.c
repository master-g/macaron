#include <stdio.h>
#include <sys/time.h>

#include "macaron/macaron.h"

int main() {
	CarromGameDef gameDef = CarromGameDefLoadFromToml("samples/config/carrom_config_example.toml");
	CarromGameState state = CarromGameState_New(&gameDef);

    const time_t now = time(NULL);

	const int steps = 250;
    for (int i = 0; i < steps; i++)
    {
    	CarromGameState_Step(&state);
    }

    const time_t elapsed_ms = (time(NULL) - now) * 1000;

    printf("Elapsed time: %ld ms\n", elapsed_ms);

	CarromGameState_Destroy(&state);

	return 0;
 }