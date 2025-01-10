#include <stdio.h>
#include <stdlib.h>

#include "macaron/macaron.h"

#include "dumper.h"

CarromGameDef load_game_def()
{
	return CarromGameDefLoadFromToml("samples/config/carrom_config_example.toml");
}

CarromGameState new_game_state(const CarromGameDef* gameDef)
{
	const CarromGameState state = CarromGameState_New(gameDef);

	CarromObjectPositionDef puckPosArr[PUCK_IDX_COUNT];
	CarromDefaultPuckPosition(gameDef->puckPhysicsDef.radius, gameDef->puckPhysicsDef.gap, PUCK_IDX_COUNT, puckPosArr);

	CarromGameState_SetPuckPosition(&state, PUCK_IDX_COUNT, puckPosArr);

	for (int i = 0; i < 16; i++)
	{
		CarromGameState_Step(&state);
	}

	return state;
}

void sample_place_puck_to_center()
{
	const CarromGameDef def = load_game_def();
	const CarromGameState state = new_game_state(&def);

	CarromGameState_PlacePuckToCenter(&state, 10);

	dump_game_state_to_png(&state, "sample_place_puck_to_center.png");
}

void sample_place_striker()
{
	const CarromGameDef def = load_game_def();
	const CarromGameState state = new_game_state(&def);
	for (int i = 0; i < 10; i++)
	{
		b2Vec2 pos = b2Vec2_zero;
		pos.y = state.strikerLimitDef.centerOffset;
		const float sign = i % 2 == 0 ? -1.f : 1.f;
		const float dist = state.strikerPhysicsDef.radius * 2.f + state.puckPhysicsDef.radius;
		pos.x = sign * dist * (float)i / 2;

		CarromGameState_PlacePuckToPosUnsafe(&state, i, pos);
	}

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, b2Vec2_zero);
	CarromGameState_EnableStriker(&state);
	dump_game_state_to_png(&state, "sample_place_striker_top.png");

	for (int i = 0; i < 10; i++)
	{
		b2Vec2 pos = b2Vec2_zero;
		pos.y = -state.strikerLimitDef.centerOffset;
		const float sign = i % 2 == 0 ? -1.f : 1.f;
		const float dist = state.strikerPhysicsDef.radius * 2.f + state.puckPhysicsDef.radius;
		pos.x = sign * dist * (float)i / 2;

		CarromGameState_PlacePuckToPosUnsafe(&state, i, pos);
	}

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Bottom, b2Vec2_zero);
	dump_game_state_to_png(&state, "sample_place_striker_bottom.png");

	for (int i = 0; i < 10; i++)
	{
		b2Vec2 pos = b2Vec2_zero;
		pos.x = -state.strikerLimitDef.centerOffset;
		const float sign = i % 2 == 0 ? -1.f : 1.f;
		const float dist = state.strikerPhysicsDef.radius * 2.f + state.puckPhysicsDef.radius;
		pos.y = sign * dist * (float)i / 2;

		CarromGameState_PlacePuckToPosUnsafe(&state, i, pos);
	}

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Left, b2Vec2_zero);
	dump_game_state_to_png(&state, "sample_place_striker_left.png");

	for (int i = 0; i < 10; i++)
	{
		b2Vec2 pos = b2Vec2_zero;
		pos.x = state.strikerLimitDef.centerOffset;
		const float sign = i % 2 == 0 ? -1.f : 1.f;
		const float dist = state.strikerPhysicsDef.radius * 2.f + state.puckPhysicsDef.radius;
		pos.y = sign * dist * (float)i / 2;

		CarromGameState_PlacePuckToPosUnsafe(&state, i, pos);
	}

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Right, b2Vec2_zero);
	dump_game_state_to_png(&state, "sample_place_striker_right.png");
}

void sample_strike()
{
	system("rm -r output");
	system("mkdir -p output");

	const CarromGameDef def = load_game_def();
	const CarromGameState state = new_game_state(&def);

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, b2Vec2_zero);
	const b2Vec2 impulse = {0.0f, -150.0f};
	CarromGameState_Strike(&state, impulse);

	const int steps = 1024;
	for (int i = 0; i < steps; i++)
	{
		char filepathBuffer[256];
		snprintf(filepathBuffer, sizeof(filepathBuffer), "output/carrom_strike_%03d.png", i);
		dump_game_state_to_png(&state, filepathBuffer);

		CarromGameState_Step(&state);

		const b2BodyEvents events = b2World_GetBodyEvents(state.worldId);
		if (events.moveCount == 0)
		{
			CarromGameState_DisableStriker(&state);
			printf("no more moves, break early, steps: %d\n", i);

			// dump last frame
			snprintf(filepathBuffer, sizeof(filepathBuffer), "output/carrom_strike_%03d.png", i);
			dump_game_state_to_png(&state, filepathBuffer);

			break;
		}
	}

	// make gif
	system("magick -delay 2 -loop 0 output/*.png output.gif");
}

void sample_take_snapshot()
{
	const CarromGameDef def = load_game_def();
	CarromGameState state = new_game_state(&def);

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, b2Vec2_zero);
	const b2Vec2 impulse = {0.0f, -150.0f};
	CarromGameState_Strike(&state, impulse);

	const int steps = 1024;
	for (int i = 0; i < steps; i++)
	{
		CarromGameState_Step(&state);

		const b2BodyEvents events = b2World_GetBodyEvents(state.worldId);
		if (events.moveCount == 0)
		{
			CarromGameState_DisableStriker(&state);
			printf("no more moves, break early, steps: %d\n", i);

			break;
		}
	}

	// dump last frame
	dump_game_state_to_png(&state, "sample_take_snapshot_0.png");

	// take snapshot
	CarromFrame snapshot = CarromGameState_TakeSnapshot(&state);

	// apply on old state
	CarromGameState_ApplySnapshot(&state, &snapshot, true);
	dump_game_state_to_png(&state, "sample_take_snapshot_1.png");

	// new state
	CarromGameState newState1 = new_game_state(&def);
	// apply snapshot
	CarromGameState_ApplySnapshot(&newState1, &snapshot, false);
	// dump new state
	dump_game_state_to_png(&newState1, "sample_take_snapshot_2.png");

	// new state
	CarromGameState newState2 = new_game_state(&def);
	// apply snapshot
	CarromGameState_ApplySnapshot(&newState2, &snapshot, true);
	// dump new state
	dump_game_state_to_png(&newState2, "sample_take_snapshot_3.png");
}

b2Vec2 find_pocket_impulse(const CarromTablePosition tablePos, const int numOfGoals)
{
	b2Vec2 initialImpulse = b2Vec2_zero;
	if (tablePos == CarromTablePosition_Top)
	{
		initialImpulse.x = -150.0f;
	}
	else if (tablePos == CarromTablePosition_Bottom)
	{
		initialImpulse.x = 150.0f;
	}
	else if (tablePos == CarromTablePosition_Left)
	{
		initialImpulse.y = 150.0f;
	}
	else if (tablePos == CarromTablePosition_Right)
	{
		initialImpulse.y = -150.0f;
	}

	const float rotStep = M_PI / 180.0f;
	float rot = 0.0f;

	const CarromGameDef def = load_game_def();
	while (true)
	{
		const b2Rot bRot = b2MakeRot(rot);
		rot += rotStep;

		const b2Vec2 impulse = b2RotateVector(bRot, initialImpulse);

		const CarromGameState state = new_game_state(&def);
		// strike!
		CarromGameState_PlaceStriker(&state, tablePos, b2Vec2_zero);
		CarromGameState_Strike(&state, impulse);

		// eval
		const CarromEvalResult result = CarromGameState_Eval(&state, 0);
		if (!result.strikerHitPocket && result.pucksHitPocket >= numOfGoals)
		{
			return impulse;
		}
	}
}

void sample_eval()
{
	const b2Vec2 impulse = {-28.621f, -148.244f};

	// new state
	const CarromGameDef def = load_game_def();
	const CarromGameState state = new_game_state(&def);
	CarromEvalResultViewer viewer = CarromEvalResultViewerCreate(&state);

	// strike!
	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, b2Vec2_zero);
	CarromGameState_Strike(&state, impulse);

	// eval
	const CarromEvalResult result = CarromGameState_Eval(&state, 0);
	printf("frame count: %d\n", result.numFrames);

	for (int i = 0; i < result.numFrames; i++)
	{
		const CarromFrame* frame = &result.frames[i];
		CarromEvalResultViewer_Update(&viewer, frame);
	}
}

b2Vec2 sample_eval_any(const CarromTablePosition tablePos)
{
	// const b2Vec2 impulse = find_pocket_impulse(tablePos);
	// printf("impulse found: (%.3f, %.3f)\n", impulse.x, impulse.y);

	system("rm -r output");
	system("mkdir -p output");

	const b2Vec2 impulse = {-148.540f, 20.876f};

	// new state
	const CarromGameDef def = load_game_def();
	const CarromGameState state = new_game_state(&def);

	// strike!
	CarromGameState_PlaceStriker(&state, tablePos, b2Vec2_zero);
	CarromGameState_Strike(&state, impulse);

	// dump
	CarromEvalResultViewer viewer = CarromEvalResultViewerCreate(&state);
	dump_viewer_to_png(&viewer, "output/viewer_000.png");

	// eval
	const CarromEvalResult result = CarromGameState_Eval(&state, 0);
	printf("frame count: %d, pucksHitPocket: %d, strikerHitPocket: %d\n", result.numFrames, result.pucksHitPocket,
	       result.strikerHitPocket);

	for (int i = 0; i < result.numFrames; i++)
	{
		const CarromFrame* frame = &result.frames[i];
		CarromEvalResultViewer_Update(&viewer, frame);

		if (viewer.enables[0] == false)
		{
			printf("puck 0 is disabled\n");
		}

		char buf[256];
		snprintf(buf, sizeof(buf), "output/viewer_%03d.png", i+1);
		dump_viewer_to_png(&viewer, buf);
	}

	system("magick -delay 2 -loop 0 output/*.png output.gif");

	return impulse;
}

void sample_eval_with_picture_output()
{
	const b2Vec2 impulse = find_pocket_impulse(CarromTablePosition_Bottom, 2);
	printf("impulse found: (%.3f, %.3f)\n", impulse.x, impulse.y);

	system("rm -r output");
	system("mkdir -p output");

	// const b2Vec2 impulse = {-28.621f, -148.244f};

	// new state
	const CarromGameDef def = load_game_def();
	const CarromGameState state = new_game_state(&def);

	// strike!
	CarromGameState_PlaceStriker(&state, CarromTablePosition_Bottom, b2Vec2_zero);
	CarromGameState_Strike(&state, impulse);

	CarromEvalResultViewer viewer = CarromEvalResultViewerCreate(&state);

	// eval
	const CarromEvalResult result = CarromGameState_Eval(&state, 0);
	printf("frame count: %d\n", result.numFrames);

	// dump
	dump_viewer_to_png(&viewer, "output/viewer_000.png");

	for (int i = 0; i < result.numFrames; i++)
	{
		const CarromFrame* frame = &result.frames[i];
		CarromEvalResultViewer_Update(&viewer, frame);

		char buf[256];
		snprintf(buf, sizeof(buf), "output/viewer_%03d.png", i+1);
		dump_viewer_to_png(&viewer, buf);
	}

	system("magick -delay 2 -loop 0 output/*.png output.gif");
}

int main(int argc, char** argv)
{
	sample_take_snapshot();
	// sample_userdata();
	// sample_eval();
	// sample_eval_with_picture_output();
	// sample_place_striker();
	// sample_eval_any(CarromTablePosition_Left);

	return 0;
}
