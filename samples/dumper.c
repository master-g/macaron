#include "dumper.h"

#define OLIVEC_IMPLEMENTATION
#include "extern/olive.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "extern/stb_image_write.h"


static uint32_t pixels[IMG_WIDTH * IMG_HEIGHT];

void dump_game_state_to_png(const CarromGameState* state, const char* file_path)
{
	const float scaleX = IMG_WIDTH / state->worldDef.width;
	const float scaleY = IMG_HEIGHT / state->worldDef.height;

	const Olivec_Canvas oc = olivec_canvas(pixels, IMG_WIDTH, IMG_HEIGHT, IMG_WIDTH);
	olivec_fill(oc, 0xFF78BBF0);

	// striker limiter
	const float strikerLimitWidth = state->strikerLimitDef.width;
	const float strikerCenterOffset = state->strikerLimitDef.centerOffset;
	const int x0 = (int)(-strikerLimitWidth / 2 * scaleX + IMG_WIDTH / 2.f);
	const int x1 = (int)(strikerLimitWidth / 2 * scaleX + IMG_WIDTH / 2.f);
	const int y0 = (int)(IMG_HEIGHT / 2.f - strikerCenterOffset * scaleY);
	const int y1 = (int)(IMG_HEIGHT / 2.f + strikerCenterOffset * scaleY);
	olivec_line(oc, x0, y0, x1, y0, 0xFF000000);
	olivec_line(oc, x0, y1, x1, y1, 0xFF000000);

	// pockets
	for (int i = 0; i < state->numOfPockets; i++)
	{
		const b2ShapeId shapeId = state->pockets[i];
		const b2Circle pocket = b2Shape_GetCircle(shapeId);

		const int x = (int)(pocket.center.x * scaleX + IMG_WIDTH / 2.f);
		const int y = (int)(IMG_HEIGHT / 2.f - pocket.center.y * scaleY);
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
		const int x = (int)(pos.x * scaleX + IMG_WIDTH / 2.f);
		const int y = (int)(IMG_HEIGHT / 2.f - pos.y * scaleY);

		olivec_circle(oc, x, y, puckR, puckColors[puck->color]);
	}

	// striker
	if (b2Body_IsEnabled(state->strikerBodyId))
	{
		const b2Vec2 pos = b2Body_GetPosition(state->strikerBodyId);
		const int x = (int)(pos.x * scaleX + IMG_WIDTH / 2.f);
		const int y = (int)(IMG_HEIGHT / 2.f - pos.y * scaleY);
		const int r = (int)(state->strikerPhysicsDef.radius * scaleX);

		olivec_circle(oc, x, y, r, 0xFFB74C34);
	}

	if (!stbi_write_png(file_path, IMG_WIDTH, IMG_HEIGHT, 4, pixels, sizeof(uint32_t) * IMG_WIDTH))
	{
		fprintf(stderr, "ERROR: could not write %s\n", file_path);
	}
}