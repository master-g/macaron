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

static CarromGameDef CarromGameDefLoadFromToml(const char* path)
{
	FILE* fp = NULL;
	char errbuf[200];

	fp = fopen(path, "r");
	if (!fp)
	{
		error("cannot open config toml - ", strerror(errno));
	}

	toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
	fclose(fp);

	if (!conf)
	{

	}
}
