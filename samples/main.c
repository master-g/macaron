#include <stdio.h>
#include <stdlib.h>

#include "macaron/macaron.h"

#include "dumper.h"

CarromGameState new_game_state()
{
	const CarromGameDef gameDef = CarromGameDefLoadFromToml("samples/config/carrom_config_example.toml");
	// for (int i = 0; i < gameDef.numOfPucks; i++)
	// {
	// 	const CarromPuckPositionDef posDef = gameDef.pucksPositions[i];
	// 	printf("idx: %d, color: %d, pos: (%.3f, %.3f)\n", posDef.index, posDef.color, posDef.position.x, posDef.position.y);
	// }
	const CarromGameState state = CarromGameState_New(&gameDef);

	for (int i = 0; i < 8; i++)
	{
		CarromGameState_Step(&state);
	}

	return state;
}

void sample_place_puck_to_center()
{
	const CarromGameState state = new_game_state();

	CarromGameState_PlacePuckToCenter(&state, 10);

	dump_game_state_to_png(&state, "sample_place_puck_to_center.png");
}

void sample_place_striker()
{
	const CarromGameState state = new_game_state();
	for (int i = 0; i < 10; i++)
	{
		b2Vec2 pos = b2Vec2_zero;
		pos.y = state.strikerLimitDef.centerOffset;
		const float sign = i % 2 == 0 ? -1.f : 1.f;
		const float dist = state.strikerPhysicsDef.radius * 2.f + state.puckPhysicsDef.radius;
		pos.x = sign * dist * (float)i / 2;

		CarromGameState_PlacePuckToPosUnsafe(&state, i, pos);
	}

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, 0.0f);

	b2Body_Enable(state.strikerBodyId);

	dump_game_state_to_png(&state, "sample_place_striker.png");
}

void sample_strike()
{
	system("rm -r output");
	system("mkdir -p output");

	const CarromGameState state = new_game_state();

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, 0.0f);
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
			b2Body_Disable(state.strikerBodyId);
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
	CarromGameState state = new_game_state();

	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, 0.0f);
	const b2Vec2 impulse = {0.0f, -150.0f};
	CarromGameState_Strike(&state, impulse);

	const int steps = 1024;
	for (int i = 0; i < steps; i++)
	{
		CarromGameState_Step(&state);

		const b2BodyEvents events = b2World_GetBodyEvents(state.worldId);
		if (events.moveCount == 0)
		{
			b2Body_Disable(state.strikerBodyId);
			printf("no more moves, break early, steps: %d\n", i);

			break;
		}
	}

	// dump last frame
	dump_game_state_to_png(&state, "sample_take_snapshot_0.png");

	// take snapshot
	CarromSnapshot snapshot = CarromGameState_TakeSnapshot(&state);

	// apply on old state
	CarromGameState_ApplySnapshot(&state, &snapshot, true);
	dump_game_state_to_png(&state, "sample_take_snapshot_1.png");

	// new state
	CarromGameState newState1 = new_game_state();
	// apply snapshot
	CarromGameState_ApplySnapshot(&newState1, &snapshot, false);
	// dump new state
	dump_game_state_to_png(&newState1, "sample_take_snapshot_2.png");

	// new state
	CarromGameState newState2 = new_game_state();
	// apply snapshot
	CarromGameState_ApplySnapshot(&newState2, &snapshot, true);
	// dump new state
	dump_game_state_to_png(&newState2, "sample_take_snapshot_3.png");
}

b2Vec2 find_pocket_impulse()
{
	const b2Vec2 initialImpulse = {-150.0f, 0.0f};
	const float rotStep = M_PI / 180.0f;
	float rot = 0.0f;

	while (true)
	{
		const b2Rot bRot = b2MakeRot(rot);
		rot += rotStep;

		const b2Vec2 impulse = b2RotateVector(bRot, initialImpulse);

		const CarromGameState state = new_game_state();
		// strike!
		CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, 0.0f);
		CarromGameState_Strike(&state, impulse);

		// eval
		const CarromEvalResult result = CarromGameState_Eval(&state, 0);

		for (int i = 0; i < result.frameCount; i++)
		{
			const CarromFrame* frame = &result.frames[i];
			for (int j = 0; j < frame->movementCount; j++)
			{
				const CarromObjectMovement* movement = &frame->movements[j];
				if (movement->hitPocket && movement->type == CarromObjectType_Puck)
				{
					return impulse;
				}
			}
		}
	}
}

void sample_eval()
{
	const b2Vec2 impulse = {-28.621f, -148.244f};

	// new state
	const CarromGameState state = new_game_state();
	CarromEvalResultViewer viewer = CarromEvalResultViewerCreate(&state);

	// strike!
	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, 0.0f);
	CarromGameState_Strike(&state, impulse);

	// eval
	const CarromEvalResult result = CarromGameState_Eval(&state, 0);
	printf("frame count: %d\n", result.frameCount);

	for (int i = 0; i < result.frameCount; i++)
	{
		const CarromFrame* frame = &result.frames[i];
		CarromEvalResultViewer_Update(&viewer, frame);
	}
}

void sample_eval_with_picture_output()
{
	// b2Vec2 impulse = find_pocket_impulse();
	// printf("impulse found: (%.3f, %.3f)\n", impulse.x, impulse.y);

	system("rm -r output");
	system("mkdir -p output");

	const b2Vec2 impulse = {-28.621f, -148.244f};

	// new state
	const CarromGameState state = new_game_state();
	CarromEvalResultViewer viewer = CarromEvalResultViewerCreate(&state);

	// strike!
	CarromGameState_PlaceStriker(&state, CarromTablePosition_Top, 0.0f);
	CarromGameState_Strike(&state, impulse);

	// eval
	const CarromEvalResult result = CarromGameState_Eval(&state, 0);
	printf("frame count: %d\n", result.frameCount);

	// dump
	dump_viewer_to_png(&viewer, "output/viewer_000.png");

	for (int i = 0; i < result.frameCount; i++)
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
	// sample_take_snapshot();
	// sample_userdata();
	// sample_eval();
	sample_eval_with_picture_output();

	return 0;
}
