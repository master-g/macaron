#pragma once

#include <stddef.h>

#include <box2d/box2d.h>
#include <box2d/types.h>
#include <box2d/math_functions.h>

#define MACARON_API B2_API
#define MACARON_INLINE B2_INLINE

typedef int MacaronAssertFcn(const char* condition, const char* fileName, int lineNumber);
