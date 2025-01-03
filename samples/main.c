#include <stdio.h>
#include <time.h>

#include "macaron/macaron.h"

#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 512
#define HEIGHT 512

uint32_t pixels[WIDTH * HEIGHT];

static void dump_game_state_to_png(const CarromGameState* state, const char* file_path)
{
	const float scaleX = WIDTH / state->worldDef.width;
	const float scaleY = HEIGHT / state->worldDef.height;

	Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
	olivec_fill(oc, 0xFF78BBF0);
	// draw pucks
	const int puckR = (int)(state->puckPhysicsDef.radius * scaleX);
	const uint32_t puckColors[] = {
		0xFFFCFAF8, // white
		0xFF101013, // black
		0xFF5B2CF7, // red
	};
	for (int i = 0; i < state->numOfPucks; i++)
	{
		const CarromPuck* puck = &state->pucks[i];
		const b2Vec2 pos = CarromPuck_GetPosition(&state->pucks[i]);
		const int x = (int)(pos.x * scaleX + WIDTH / 2.f);
		const int y = (int)(pos.y * scaleY + HEIGHT / 2.f);

		olivec_circle(oc, x, y, puckR, puckColors[puck->color]);
	}

	for (int i = 0; i < state->numOfPockets; i++)
	{
		const b2ShapeId shapeId = state->pockets[i];
		const b2Circle pocket = b2Shape_GetCircle(shapeId);

		const int x = (int)(pocket.center.x * scaleX + WIDTH / 2.f);
		const int y = (int)(pocket.center.y * scaleY + HEIGHT / 2.f);
		const int r = (int)(pocket.radius * scaleX);

		olivec_circle(oc, x, y, r, 0xFF7FC416);
	}

	if (!stbi_write_png(file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t) * WIDTH))
	{
		fprintf(stderr, "ERROR: could not write %s\n", file_path);
	}
}

int main()
{
	const char* imageDir = "output";
	const char* mkdirCmd = "mkdir -p output";
	system(mkdirCmd);

	const CarromGameDef gameDef = CarromGameDefLoadFromToml("samples/config/carrom_config_example.toml");
	for (int i = 0; i < gameDef.numOfPucks; i++)
	{
		const CarromPuckPositionDef posDef = gameDef.pucksPositions[i];
		printf("idx: %d, color: %d, pos: (%.3f, %.3f)\n", posDef.index, posDef.color, posDef.position.x, posDef.position.y);
	}
	CarromGameState state = CarromGameState_New(&gameDef);

	bool exploded = false;

	struct timespec start, end;
	timespec_get(&start, TIME_UTC);

	const int steps = 512;
	for (int i = 0; i < steps; i++)
	{
		char filepathBuffer[256];
		CarromGameState_Step(&state);

		snprintf(filepathBuffer, sizeof(filepathBuffer), "output/carrom_%03d.png", i);
		dump_game_state_to_png(&state, filepathBuffer);

		if (!exploded && i == 10)
		{
			exploded = true;
			b2World_Explode(state.worldId, b2Vec2_zero, 0.2f, .5f);
		}

		const b2BodyEvents events = b2World_GetBodyEvents(state.worldId);
		if (exploded && events.moveCount == 0)
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
		const CarromPuck* puck = &state.pucks[i];
		if (CarromPuck_IsEnable(puck))
		{
			const b2Vec2 pos = CarromPuck_GetPosition(puck);
			printf("puck %d pos: (%.3f, %.3f)\n", puck->index, pos.x, pos.y);
		}
	}

	printf("Elapsed time: %.9f s\n", elapsed);

	CarromGameState_Destroy(&state);

	return 0;
}
