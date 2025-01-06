#include <stdio.h>
#include <time.h>

#include "macaron/macaron.h"

#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 1024
#define HEIGHT 1024

uint32_t pixels[WIDTH * HEIGHT];

static void dump_game_state_to_png(const CarromGameState* state, const char* file_path)
{
	const float scaleX = WIDTH / state->worldDef.width;
	const float scaleY = HEIGHT / state->worldDef.height;

	const Olivec_Canvas oc = olivec_canvas(pixels, WIDTH, HEIGHT, WIDTH);
	olivec_fill(oc, 0xFF78BBF0);

	// striker limiter
	const float strikerLimitWidth = state->strikerLimitDef.width;
	const float strikerCenterOffset = state->strikerLimitDef.centerOffset;
	const int x0 = (int)(-strikerLimitWidth / 2 * scaleX + WIDTH / 2.f);
	const int x1 = (int)(strikerLimitWidth / 2 * scaleX + WIDTH / 2.f);
	const int y0 = (int)(HEIGHT / 2.f - strikerCenterOffset * scaleY);
	const int y1 = (int)(HEIGHT / 2.f + strikerCenterOffset * scaleY);
	olivec_line(oc, x0, y0, x1, y0, 0xFF000000);
	olivec_line(oc, x0, y1, x1, y1, 0xFF000000);

	// pockets
	for (int i = 0; i < state->numOfPockets; i++)
	{
		const b2ShapeId shapeId = state->pockets[i];
		const b2Circle pocket = b2Shape_GetCircle(shapeId);

		const int x = (int)(pocket.center.x * scaleX + WIDTH / 2.f);
		const int y = (int)(HEIGHT / 2.f - pocket.center.y * scaleY);
		const int r = (int)(pocket.radius * scaleX);

		olivec_circle(oc, x, y, r, 0xFF7FC416);
	}
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
		const int y = (int)(HEIGHT / 2.f - pos.y * scaleY);

		olivec_circle(oc, x, y, puckR, puckColors[puck->color]);
	}

	// striker
	if (b2Body_IsEnabled(state->strikerBodyId))
	{
		const b2Vec2 pos = b2Body_GetPosition(state->strikerBodyId);
		const int x = (int)(pos.x * scaleX + WIDTH / 2.f);
		const int y = (int)(HEIGHT / 2.f - pos.y * scaleY);
		const int r = (int)(state->strikerPhysicsDef.radius * scaleX);

		olivec_circle(oc, x, y, r, 0xFFDCABDF);
	}

	if (!stbi_write_png(file_path, WIDTH, HEIGHT, 4, pixels, sizeof(uint32_t) * WIDTH))
	{
		fprintf(stderr, "ERROR: could not write %s\n", file_path);
	}
}

CarromGameState new_game_state()
{
	const CarromGameDef gameDef = CarromGameDefLoadFromToml("samples/config/carrom_config_example.toml");
	for (int i = 0; i < gameDef.numOfPucks; i++)
	{
		const CarromPuckPositionDef posDef = gameDef.pucksPositions[i];
		printf("idx: %d, color: %d, pos: (%.3f, %.3f)\n", posDef.index, posDef.color, posDef.position.x, posDef.position.y);
	}
	CarromGameState state = CarromGameState_New(&gameDef);

	for (int i = 0; i < 8; i++)
	{
		CarromGameState_Step(&state);
	}

	return state;
}

int sample_make_gif()
{
	const char* mkdirCmd = "mkdir -p output";
	system(mkdirCmd);

	CarromGameState state = new_game_state();

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

	double elapsed = (double)(end.tv_sec - start.tv_sec);
	elapsed += (double)(end.tv_nsec - start.tv_nsec) / 1e9;

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

int sample_place_puck_to_center()
{
	CarromGameState state = new_game_state();

	CarromGameState_PlacePuckToCenter(&state, 10);

	dump_game_state_to_png(&state, "sample_place_puck_to_center.png");

	return 0;
}

int sample_place_striker()
{
	CarromGameState state = new_game_state();
	for (int i = 0; i < 10; i++)
	{
		CarromPuck* puck = &state.pucks[i];
		b2Vec2 pos = b2Vec2_zero;
		pos.y = state.strikerLimitDef.centerOffset;
		const float sign = i % 2 == 0 ? -1.f : 1.f;
		const float dist = state.strikerPhysicsDef.radius * 2.f + state.puckPhysicsDef.radius;
		pos.x = sign * dist * (float)i / 2;

		CarromPuck_SetPosition(puck, pos);
		CarromGameState_PlacePuck(&state, puck, 0.001f);
	}

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, 0.0f);

	b2Body_Enable(state.strikerBodyId);

	dump_game_state_to_png(&state, "sample_place_striker.png");
}

int main()
{
	return sample_place_striker();
}
