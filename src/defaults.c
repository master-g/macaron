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

CarromGameDef CarromDefaultGameDef(void)
{
	CarromGameDef def = {0};
	def.worldDef = CarromDefaultWorldDef();
	def.puckPhysicsDef = CarromDefaultPuckPhysicsDef();
	def.strikerPhysicsDef = CarromDefaultStrikerPhysicsDef();
	def.pocketDef = CarromDefaultPocketDef();
	def.strikerLimitDef = CarromDefaultStrikerLimitDef();
	def.numOfPucks = 19;

	const float outR = def.puckPhysicsDef.radius + 0.012f;
	const CarromPuckPositionDef posDefs[] = {
		{1, PuckColor_Red, {0.0f, 0.0f}},

		{2, PuckColor_White, {0.0f, outR * 4}},
		{3, PuckColor_White, {0.0f, -outR * 2}},
		{4, PuckColor_White, {0.0f, -outR * 4}},
		{5, PuckColor_White, {-outR * 2 * SPI3, outR * 2 * 0.5f}},
		{6, PuckColor_White, {-outR * 4 * SPI3, outR * 4 * 0.5f}},
		{7, PuckColor_White, {outR * 2 * SPI3, outR * 2 * 0.5f}},
		{8, PuckColor_White, {outR * 4 * SPI3, outR * 4 * 0.5f}},
		{9, PuckColor_White, {-outR * 4 * SPI3, -outR * 4 * 0.5f}},
		{10, PuckColor_White, {outR * 4 * SPI3, -outR * 4 * 0.5f}},

		{11, PuckColor_Black, {0.0f, outR * 2}},
		{12, PuckColor_Black, {-outR * 4 * SPI3, 0.0f}},
		{13, PuckColor_Black, {outR * 4 * SPI3, 0.0f}},
		{14, PuckColor_Black, {-outR * 2 * SPI3, outR * 2 * 0.5f + outR * 2}},
		{15, PuckColor_Black, {-outR * 2 * SPI3, -outR * 2 * 0.5f}},
		{16, PuckColor_Black, {-outR * 2 * SPI3, -outR * 2 * 0.5f - outR * 2}},
		{17, PuckColor_Black, {outR * 2 * SPI3, -outR * 2 * 0.5f}},
		{18, PuckColor_Black, {outR * 2 * SPI3, -outR * 2 * 0.5f - outR * 2}},
		{19, PuckColor_Black, {outR * 2 * SPI3, outR * 2 * 0.5f + outR * 2}},
	};

	for (int i = 0; i < 19; i++)
	{
		def.pucksPositions[i] = posDefs[i];
	}

	return def;
}
