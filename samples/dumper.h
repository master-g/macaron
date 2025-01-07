#pragma once

#include <macaron/macaron.h>

#define IMG_WIDTH 512
#define IMG_HEIGHT 512

extern void dump_game_state_to_png(const CarromGameState* state, const char* file_path);

extern void dump_viewer_to_png(const CarromEvalResultViewer* viewer, const char* file_path);