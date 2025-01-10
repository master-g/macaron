#include "dumper.h"

#include <macaron/macaron.h>

#define OLIVEC_IMPLEMENTATION
#include "extern/olive.c"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "extern/stb_image_write.h"

static uint32_t pixels[IMG_WIDTH * IMG_HEIGHT];

void draw_limiter(const Olivec_Canvas* oc, const float width, const float offCenter, const float scaleX, const float scaleY)
{
	// striker limiter
	const int topLeftX = (int)(-width / 2 * scaleX + IMG_WIDTH / 2.f);
	const int topRightX = (int)(width / 2 * scaleX + IMG_WIDTH / 2.f);
	const int topY = (int)(IMG_HEIGHT / 2.f - offCenter * scaleY);
	const int bottomY = (int)(IMG_HEIGHT / 2.f + offCenter * scaleY);
	olivec_line(*oc, topLeftX, topY, topRightX, topY, 0xFF000000);
	olivec_line(*oc, topLeftX, bottomY, topRightX, bottomY, 0xFF000000);

	const int leftX = (int)(IMG_WIDTH / 2.f - offCenter * scaleX);
	const int rightX = (int)(IMG_WIDTH / 2.f + offCenter * scaleX);
	const int leftTopY = (int)(-width / 2 * scaleY + IMG_HEIGHT / 2.f);
	const int leftBottomY = (int)(width / 2 * scaleY + IMG_HEIGHT / 2.f);
	olivec_line(*oc, leftX, leftTopY, leftX, leftBottomY, 0xFF000000);
	olivec_line(*oc, rightX, leftTopY, rightX, leftBottomY, 0xFF000000);
}

void dump_game_state_to_png(const CarromGameState* state, const char* file_path)
{
	const float scaleX = IMG_WIDTH / state->worldDef.width;
	const float scaleY = IMG_HEIGHT / state->worldDef.height;

	const Olivec_Canvas oc = olivec_canvas(pixels, IMG_WIDTH, IMG_HEIGHT, IMG_WIDTH);
	olivec_fill(oc, 0xFF78BBF0);

	// striker limiter
	draw_limiter(&oc, state->strikerLimitDef.width, state->strikerLimitDef.centerOffset, scaleX, scaleY);

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
	for (int i = 0; i < PUCK_IDX_COUNT; i++)
	{
		const int idx = sPuckIndexes[i];
		const b2Vec2 pos = CarromGameState_GetPuckPosition(state, idx);
		const CarromPuckColor color = MACARON_IDX_PUCK_COLOR(idx);
		const int x = (int)(pos.x * scaleX + IMG_WIDTH / 2.f);
		const int y = (int)(IMG_HEIGHT / 2.f - pos.y * scaleY);

		olivec_circle(oc, x, y, puckR, puckColors[color]);
	}

	// striker
	if (CarromGameState_IsStrikerEnabled(state))
	{
		const b2Vec2 pos =
			CarromGameState_GetStrikerPosition(state);
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

void dump_viewer_to_png(const CarromEvalResultViewer* viewer, const char* file_path)
{
	const float scaleX = IMG_WIDTH / viewer->worldWidth;
	const float scaleY = IMG_HEIGHT / viewer->worldHeight;

	const Olivec_Canvas oc = olivec_canvas(pixels, IMG_WIDTH, IMG_HEIGHT, IMG_WIDTH);
	olivec_fill(oc, 0xFF78BBF0);

	// striker limiter
	draw_limiter(&oc, viewer->strikerLimitWidth, viewer->strikerLimitOffCenter, scaleX, scaleY);

	// pockets
	for (int i = 0; i < viewer->numOfPockets; i++)
	{
		const b2Vec2 pocket = viewer->pocketsPosition[i];
		const float radius = viewer->radius[CarromObjectType_Pocket];

		const int x = (int)(pocket.x * scaleX + IMG_WIDTH / 2.f);
		const int y = (int)(IMG_HEIGHT / 2.f - pocket.y * scaleY);
		const int r = (int)(radius * scaleX);

		olivec_circle(oc, x, y, r, 0xFF7FC416);
	}
	// draw pucks
	const int puckR = (int)(viewer->radius[CarromObjectType_Puck] * scaleX);
	const uint32_t puckColors[] = {
		0xFFFCFAF8, // white
		0xFF101013, // black
		0xFF5B2CF7, // red
	};
	for (int i = 0; i < PUCK_IDX_COUNT; i++)
	{
		const int idx = sPuckIndexes[i];
		if (!viewer->enables[idx])
		{
			continue;
		}
		const b2Vec2 pos = viewer->positions[idx];
		const CarromPuckColor color = MACARON_IDX_PUCK_COLOR(idx);

		const int x = (int)(pos.x * scaleX + IMG_WIDTH / 2.f);
		const int y = (int)(IMG_HEIGHT / 2.f - pos.y * scaleY);

		uint32_t argb = puckColors[color];
		if (viewer->rest[idx])
		{
			argb &= 0x00FFFFFF;
			argb |= 0xA0000000;
		}

		olivec_circle(oc, x, y, puckR, argb);
	}

	// striker
	if (viewer->enables[IDX_STRIKER])
	{
		const b2Vec2 pos = viewer->positions[IDX_STRIKER];
		const int x = (int)(pos.x * scaleX + IMG_WIDTH / 2.f);
		const int y = (int)(IMG_HEIGHT / 2.f - pos.y * scaleY);
		const int r = (int)(viewer->radius[CarromObjectType_Striker] * scaleX);

		olivec_circle(oc, x, y, r, 0xFFB74C34);
	}

	if (!stbi_write_png(file_path, IMG_WIDTH, IMG_HEIGHT, 4, pixels, sizeof(uint32_t) * IMG_WIDTH))
	{
		fprintf(stderr, "ERROR: could not write %s\n", file_path);
	}
}
