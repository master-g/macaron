#include "core.h"

#include <macaron/types.h>

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

CarromPuckPhysicsDef CarromDefaultPuckPhysicsDef(void)
{
	CarromPuckPhysicsDef def = {0};
	def.radius = 0.16f;
	def.gap = 0.0001f;
	def.bodyLinearDamping = 1.8f;
	def.bodyAngularDamping = 2.0f;
	def.shapeFriction = 0.0f;
	def.shapeRestitution = 1.0f;
	def.shapeDensity = 0.45f;
	return def;
}

CarromPuckPhysicsDef CarromDefaultStrikerPhysicsDef(void)
{
	CarromPuckPhysicsDef def = {0};
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

	const CarromPuckPositionDef posDefs[] = {
		{PuckColor_Red, {0.0f, 0.0f}},

		{PuckColor_White, {-outR * 4, 0.0f}},
		{PuckColor_White, {outR * 2, 0.0f}},
		{PuckColor_White, {outR * 4, 0.0f}},
		{PuckColor_White, {-outR * 2 * 0.5f, outR * 2 * SPI3}},
		{PuckColor_White, {-outR * 4 * 0.5f, outR * 4 * SPI3}},
		{PuckColor_White, {-outR * 2 * 0.5f, -outR * 2 * SPI3}},
		{PuckColor_White, {-outR * 4 * 0.5f, -outR * 4 * SPI3}},
		{PuckColor_White, {outR * 4 * 0.5f, -outR * 4 * SPI3}},
		{PuckColor_White, {outR * 4 * 0.5f, outR * 4 * SPI3}},

		{PuckColor_Black, {-outR * 2, 0.0f}},
		{PuckColor_Black, {-outR * 3, outR * 2 * SPI3}},
		{PuckColor_Black, {-outR * 3, -outR * 2 * SPI3}},

		{PuckColor_Black, {outR, outR * 2 * SPI3}},
		{PuckColor_Black, {outR * 3, outR * 2 * SPI3}},
		{PuckColor_Black, {outR * 3, -outR * 2 * SPI3}},
		{PuckColor_Black, {outR, -outR * 2 * SPI3}},

		{PuckColor_Black, {0.0f, outR * 4 * SPI3}},
		{PuckColor_Black, {0.0f, -outR * 4 * SPI3}},
	};

	const int count = sizeof(posDefs) / sizeof(CarromPuckPositionDef);
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

	const CarromPocketPositionDef posDefs[] = {
		{{-w / 2 + r / 2, h / 2 - r / 2}},
		{{w / 2 - r / 2, h / 2 - r / 2}},
		{{-w / 2 + r / 2, -h / 2 + r / 2}},
		{{w / 2 - r / 2, -h / 2 + r / 2}},
	};

	const int count = sizeof(posDefs) / sizeof(CarromPocketPositionDef);
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
