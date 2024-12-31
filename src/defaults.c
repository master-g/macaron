#include <macaron/types.h>

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
	// TODO: place these pucks

	return def;
}
