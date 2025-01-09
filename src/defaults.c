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
	return def;
}

CarromStrikerLimitDef CarromDefaultStrikerLimitDef(void)
{
	CarromStrikerLimitDef def = {0};
	def.width = 8.0f;
	def.centerOffset = 4.0f;
	def.maxForce = 150.0f;
	return def;
}

void CarromGameDef_PlacePucks(CarromGameDef* def)
{
	MACARON_ASSERT(def != NULL);
	if (def == NULL)
	{
		return;
	}

	const float outR = def->puckPhysicsDef.radius + def->puckPhysicsDef.gap;

	MACARON_ASSERT(outR > 0.0f);

	const CarromObjectPositionDef posDefs[] = {
		{IDX_PUCK_RED, {0.0f, 0.0f}},

		{IDX_PUCK_BLACK_START + 0, {-outR * 2, 0.0f}},
		{IDX_PUCK_BLACK_START + 1, {-outR * 3, outR * 2 * SPI3}},
		{IDX_PUCK_BLACK_START + 2, {-outR * 3, -outR * 2 * SPI3}},
		{IDX_PUCK_BLACK_START + 3, {outR, outR * 2 * SPI3}},
		{IDX_PUCK_BLACK_START + 4, {outR * 3, outR * 2 * SPI3}},
		{IDX_PUCK_BLACK_START + 5, {outR * 3, -outR * 2 * SPI3}},
		{IDX_PUCK_BLACK_START + 6, {outR, -outR * 2 * SPI3}},
		{IDX_PUCK_BLACK_START + 7, {0.0f, outR * 4 * SPI3}},
		{IDX_PUCK_BLACK_START + 8, {0.0f, -outR * 4 * SPI3}},

		{IDX_PUCK_WHITE_START + 0, {-outR * 4, 0.0f}},
		{IDX_PUCK_WHITE_START + 1, {outR * 2, 0.0f}},
		{IDX_PUCK_WHITE_START + 2, {outR * 4, 0.0f}},
		{IDX_PUCK_WHITE_START + 3, {-outR * 2 * 0.5f, outR * 2 * SPI3}},
		{IDX_PUCK_WHITE_START + 4, {-outR * 4 * 0.5f, outR * 4 * SPI3}},
		{IDX_PUCK_WHITE_START + 5, {-outR * 2 * 0.5f, -outR * 2 * SPI3}},
		{IDX_PUCK_WHITE_START + 6, {-outR * 4 * 0.5f, -outR * 4 * SPI3}},
		{IDX_PUCK_WHITE_START + 7, {outR * 4 * 0.5f, -outR * 4 * SPI3}},
		{IDX_PUCK_WHITE_START + 8, {outR * 4 * 0.5f, outR * 4 * SPI3}},
	};

	const int count = sizeof(posDefs) / sizeof(CarromObjectPositionDef);
	for (int i = 0; i < count; i++)
	{
		def->pucksPositions[i] = posDefs[i];
	}
}

void CarromGameDef_PlacePockets(CarromGameDef* def)
{
	MACARON_ASSERT(def != NULL);
	if (def == NULL)
	{
		return;
	}

	const float r = def->pocketDef.radius;

	MACARON_ASSERT(r > 0.0f);

	const float w = def->worldDef.width;
	const float h = def->worldDef.height;

	MACARON_ASSERT(w > 0.0f && h > 0.0f);

	const CarromObjectPositionDef posDefs[] = {
		{-1, {-w / 2 + r / 2, h / 2 - r / 2}},
		{-1, {w / 2 - r / 2, h / 2 - r / 2}},
		{-1, {-w / 2 + r / 2, -h / 2 + r / 2}},
		{-1, {w / 2 - r / 2, -h / 2 + r / 2}},
	};

	const int count = sizeof(posDefs) / sizeof(CarromObjectPositionDef);
	for (int i = 0; i < count; i++)
	{
		def->pocketsPositions[i] = posDefs[i];
	}
}

CarromGameDef CarromDefaultGameDef(void)
{
	CarromGameDef def = {0};
	def.worldDef = CarromDefaultWorldDef();
	def.puckPhysicsDef = CarromDefaultPuckPhysicsDef();
	def.strikerPhysicsDef = CarromDefaultStrikerPhysicsDef();
	def.pocketDef = CarromDefaultPocketDef();
	def.strikerLimitDef = CarromDefaultStrikerLimitDef();
	def.numOfPucks = 19;
	def.numOfPockets = 4;

	return def;
}
