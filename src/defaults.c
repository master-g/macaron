#include "core.h"

#include <macaron/types.h>

// sine value of 60 degrees
#define SPI3 0.8660254037844386f

CarromWorldDef CarromDefaultWorldDef(void)
{
	CarromWorldDef def = {0};
	def.width = 8.81f;
	def.height = 8.81f;
	def.workerCount = 4;
	def.subStep = 4;
	def.disableSleep = false;
	def.frameDuration = 1.0f / 60;
	return def;
}

CarromObjectPhysicsDef CarromDefaultPuckPhysicsDef(void)
{
	CarromObjectPhysicsDef def = {0};
	def.radius = 0.16f;
	def.gap = 0.0001f;
	def.bodyLinearDamping = 1.8f;
	def.bodyAngularDamping = 2.0f;
	def.shapeFriction = 0.0f;
	def.shapeRestitution = 1.0f;
	def.shapeDensity = 0.45f;
	return def;
}

CarromObjectPhysicsDef CarromDefaultStrikerPhysicsDef(void)
{
	CarromObjectPhysicsDef def = {0};
	def.radius = 0.2176f;
	def.bodyLinearDamping = 2.8f;
	def.bodyAngularDamping = 5.0f;
	def.shapeFriction = 0.0f;
	def.shapeRestitution = 1.0f;
	def.shapeDensity = 0.5f;
	return def;
}

CarromPocketDef CarromDefaultPocketDef(void)
{
	CarromPocketDef def = {0};
	def.radius = 0.22f;
	def.cornerOffsetX = 0.0f;
	def.cornerOffsetY = 0.0f;
	return def;
}

CarromStrikerLimitDef CarromDefaultStrikerLimitDef(void)
{
	CarromStrikerLimitDef def = {0};
	def.width = 8.0f;
	def.centerOffset = 4.0f;
	return def;
}

CarromGameDef CarromDefaultGameDef(void)
{
	CarromGameDef def = {0};
	def.worldDef = CarromDefaultWorldDef();
	def.puckPhysicsDef = CarromDefaultPuckPhysicsDef();
	def.strikerPhysicsDef = CarromDefaultStrikerPhysicsDef();
	def.pocketDef = CarromDefaultPocketDef();
	def.strikerLimitDef = CarromDefaultStrikerLimitDef();

	return def;
}

int CarromDefaultPuckPosition(const float radius, const float gap, const int numOfPucks, CarromObjectPositionDef* positions)
{
	if (positions == NULL || numOfPucks < PUCK_IDX_COUNT)
	{
		return PUCK_IDX_COUNT;
	}

	const float outR = radius + gap;
	positions[0] = (CarromObjectPositionDef){IDX_PUCK_RED, {0.0f, 0.0f}};

	positions[1] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 0, {-outR * 2, 0.0f}};
	positions[2] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 1, {-outR * 3, outR * 2 * SPI3}};
	positions[3] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 2, {-outR * 3, -outR * 2 * SPI3}};
	positions[4] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 3, {outR, outR * 2 * SPI3}};
	positions[5] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 4, {outR * 3, outR * 2 * SPI3}};
	positions[6] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 5, {outR * 3, -outR * 2 * SPI3}};
	positions[7] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 6, {outR, -outR * 2 * SPI3}};
	positions[8] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 7, {0.0f, outR * 4 * SPI3}};
	positions[9] = (CarromObjectPositionDef){IDX_PUCK_BLACK_START + 8, {0.0f, -outR * 4 * SPI3}};

	positions[10] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 0, {-outR * 4, 0.0f}};
	positions[11] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 1, {outR * 2, 0.0f}};
	positions[12] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 2, {outR * 4, 0.0f}};
	positions[13] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 3, {-outR * 2 * 0.5f, outR * 2 * SPI3}};
	positions[14] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 4, {-outR * 4 * 0.5f, outR * 4 * SPI3}};
	positions[15] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 5, {-outR * 2 * 0.5f, -outR * 2 * SPI3}};
	positions[16] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 6, {-outR * 4 * 0.5f, -outR * 4 * SPI3}};
	positions[17] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 7, {outR * 4 * 0.5f, -outR * 4 * SPI3}};
	positions[18] = (CarromObjectPositionDef){IDX_PUCK_WHITE_START + 8, {outR * 4 * 0.5f, outR * 4 * SPI3}};

	return PUCK_IDX_COUNT;
}
