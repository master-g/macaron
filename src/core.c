#include <stdio.h>


#include <macaron/base.h>

#include "core.h"

static int MacaronDefaultAssertFcn( const char* condition, const char* fileName, int lineNumber )
{
	printf( "BOX2D ASSERTION: %s, %s, line %d\n", condition, fileName, lineNumber );

	// return non-zero to break to debugger
	return 1;
}

MacaronAssertFcn* MacaronAssertHandler = MacaronDefaultAssertFcn;

void MacaronSetAssertFcn( MacaronAssertFcn* assertFcn )
{
	MACARON_ASSERT( assertFcn != NULL );
	MacaronAssertHandler = assertFcn;
}