#include <macaron/types.h>

#define SPI3 0.8660254037844386f

CarromWorldDef CarromDefaultWorldDef(void)
{
	CarromWorldDef def = {0};
	def.width = 8.81f;
	def.height = 8.81f;
	def.workerCount = 4;
	def.subStep = 4;
	def.enableSleep = false;
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
const float outR = def->puckPhysicsDef.radius + def->puckPhysicsDef.gap;
	const CarromPuckPositionDef posDefs[] = {
		{1, PuckColor_Red, {0.0f, 0.0f}},

		{2, PuckColor_White, {-outR * 4, 0.0f}},
		{3, PuckColor_White, {outR * 2, 0.0f}},
		{4, PuckColor_White, {outR * 4, 0.0f}},
		{5, PuckColor_White, {-outR * 2 * 0.5f, outR * 2 * SPI3}},
		{6, PuckColor_White, {-outR * 4 * 0.5f, outR * 4 * SPI3}},
		{7, PuckColor_White, {-outR * 2 * 0.5f, -outR * 2 * SPI3}},
		{8, PuckColor_White, {-outR * 4 * 0.5f, -outR * 4 * SPI3}},
		{9, PuckColor_White, {outR * 4 * 0.5f, -outR * 4 * SPI3}},
		{10, PuckColor_White, {outR * 4 * 0.5f, outR * 4 * SPI3}},

		{11, PuckColor_Black, {-outR * 2, 0.0f}},
		{12, PuckColor_Black, {-outR * 3, outR * 2 * SPI3}},
		{13, PuckColor_Black, {-outR * 3, -outR * 2 * SPI3}},

		{14, PuckColor_Black, {outR, outR * 2 * SPI3}},
		{15, PuckColor_Black, {outR * 3, outR * 2 * SPI3}},
		{16, PuckColor_Black, {outR * 3, -outR * 2 * SPI3}},
		{17, PuckColor_Black, {outR, -outR * 2 * SPI3}},

		{18, PuckColor_Black, {0.0f, outR * 4 * SPI3}},
		{19, PuckColor_Black, {0.0f, -outR * 4 * SPI3}},
	};

	for (int i = 0; i < 19; i++)
	{
		def->pucksPositions[i] = posDefs[i];
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

	CarromGameDef_PlacePucks(def.pucksPositions);

	return def;
}
