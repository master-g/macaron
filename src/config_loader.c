#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <macaron/types.h>

#include "toml.h"

static void error(const char* msg, const char* msg1)
{
	fprintf(stderr, "ERROR: %s%s\n", msg, msg1 ? msg1 : "");
	exit(1);
}

CarromObjectPhysicsDef CarromPuckPhysicsDefFromTomlTable(const toml_table_t* table)
{
	CarromObjectPhysicsDef puckPhysicsDef = {0};

	const toml_datum_t puckRadius = toml_double_in(table, "radius");
	if (!puckRadius.ok)
	{
		error("cannot read puck.radius", "");
	}
	const toml_datum_t puckGap = toml_double_in(table, "gap");

	const toml_datum_t puckLinearDamping = toml_double_in(table, "bodyLinearDamping");
	if (!puckLinearDamping.ok)
	{
		error("cannot read puck.bodyLinearDamping", "");
	}
	const toml_datum_t puckAngularDamping = toml_double_in(table, "bodyAngularDamping");
	if (!puckAngularDamping.ok)
	{
		error("cannot read puck.bodyAngularDamping", "");
	}
	const toml_datum_t puckFriction = toml_double_in(table, "shapeFriction");
	if (!puckFriction.ok)
	{
		error("cannot read puck.shapeFriction", "");
	}
	const toml_datum_t puckRestitution = toml_double_in(table, "shapeRestitution");
	if (!puckRestitution.ok)
	{
		error("cannot read puck.shapeRestitution", "");
	}
	const toml_datum_t puckDensity = toml_double_in(table, "shapeDensity");
	if (!puckDensity.ok)
	{
		error("cannot read puck.shapeDensity", "");
	}

	puckPhysicsDef.radius = (float)puckRadius.u.d;
	puckPhysicsDef.gap = puckGap.ok ? (float)puckGap.u.d : 0.0f;
	puckPhysicsDef.bodyLinearDamping = (float)puckLinearDamping.u.d;
	puckPhysicsDef.bodyAngularDamping = (float)puckAngularDamping.u.d;
	puckPhysicsDef.shapeFriction = (float)puckFriction.u.d;
	puckPhysicsDef.shapeRestitution = (float)puckRestitution.u.d;
	puckPhysicsDef.shapeDensity = (float)puckDensity.u.d;

	return puckPhysicsDef;

}

CarromGameDef CarromGameDefLoadFromToml(const char* path)
{
	FILE* fp = NULL;
	char errBuf[200];

	fp = fopen(path, "r");
	if (!fp)
	{
		error("cannot open config toml - ", strerror(errno));
	}

	toml_table_t* conf = toml_parse_file(fp, errBuf, sizeof(errBuf));
	fclose(fp);

	if (!conf)
	{
		error("cannot parse config toml - ", errBuf);
	}

	// world def
	CarromWorldDef worldDef = {0};

	toml_table_t* world = toml_table_in(conf, "world");
	if (!world)
	{
		error("missing [world]", "");
	}

	const toml_datum_t width = toml_double_in(world, "width");
	if (!width.ok)
	{
		error("cannot read world.width", "");
	}
	const toml_datum_t height = toml_double_in(world, "height");
	if (!height.ok)
	{
		error("cannot read world.height", "");
	}
	const toml_datum_t workerCount = toml_int_in(world, "workerCount");
	if (!workerCount.ok)
	{
		error("cannot read world.workerCount", "");
	}
	const toml_datum_t subStep = toml_int_in(world, "subStep");
	if (!subStep.ok)
	{
		error("cannot read world.subStep", "");
	}
	const toml_datum_t disableSleep = toml_bool_in(world, "disableSleep");

	const toml_datum_t frameDuration = toml_double_in(world, "frameDuration");
	if (!frameDuration.ok)
	{
		error("cannot read world.frameDuration", "");
	}

	worldDef.width = (float)width.u.d;
	worldDef.height = (float)height.u.d;
	worldDef.workerCount = (int32_t)workerCount.u.i;
	worldDef.subStep = (int32_t)subStep.u.i;
	worldDef.disableSleep = disableSleep.ok ? disableSleep.u.b : false;
	worldDef.frameDuration = (float)frameDuration.u.d;

	// puck
	toml_table_t* puckPhysicsTable = toml_table_in(conf, "puck");
	if (!puckPhysicsTable)
	{
		error("missing [puck]", "");
	}

	CarromObjectPhysicsDef puckPhysicsDef = CarromPuckPhysicsDefFromTomlTable(puckPhysicsTable);

	// pocket
	CarromPocketDef pocketDef = {0};

	const toml_table_t* pocketTable = toml_table_in(conf, "pocket");
	if (!pocketTable)
	{
		error("missing [poket]", "");
	}
	const toml_datum_t pocketRadius = toml_double_in(pocketTable, "radius");
	if (!pocketRadius.ok)
	{
		error("cannot read pocket.radius", "");
	}
	pocketDef.radius = (float)pocketRadius.u.d;
	const toml_datum_t pocketCornerOffsetX = toml_double_in(pocketTable, "cornerOffsetX");
	if (pocketCornerOffsetX.ok)
	{
		pocketDef.cornerOffsetX = (float)pocketCornerOffsetX.u.d;
	}
	const toml_datum_t pocketCornerOffsetY = toml_double_in(pocketTable, "cornerOffsetY");
	if (pocketCornerOffsetY.ok)
    {
        pocketDef.cornerOffsetY = (float)pocketCornerOffsetY.u.d;
    }

	// striker limit
	CarromStrikerLimitDef strikerLimits = {0};

	const toml_table_t* strikerLimitTable = toml_table_in(conf, "striker_limit");
	if (!strikerLimitTable)
	{
		error("missing [striker_limit]", "");
	}
	const toml_datum_t strikerLimitWidth = toml_double_in(strikerLimitTable, "width");
	if (!strikerLimitWidth.ok)
	{
		error("cannot read striker_limit.width", "");
	}
	strikerLimits.width = (float)strikerLimitWidth.u.d;

	const toml_datum_t strikerCenterOffset = toml_double_in(strikerLimitTable, "centerOffset");
	if (!strikerCenterOffset.ok)
	{
		error("cannot read striker_limit.centerOffset", "");
	}
	strikerLimits.centerOffset = (float)strikerCenterOffset.u.d;

	// striker physics def
	toml_table_t* strikerPhysicsTable = toml_table_in(conf, "striker");
	if (!strikerPhysicsTable)
	{
		error("missing [striker]", "");
	}

	CarromObjectPhysicsDef strikerPhysicsDef = CarromPuckPhysicsDefFromTomlTable(strikerPhysicsTable);

	toml_free(conf);

	CarromGameDef def = CarromDefaultGameDef();
	def.worldDef = worldDef;
	def.puckPhysicsDef = puckPhysicsDef;
	def.pocketDef = pocketDef;
	def.strikerLimitDef = strikerLimits;
	def.strikerPhysicsDef = strikerPhysicsDef;

	return def;
}
