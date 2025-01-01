#pragma once

#include "../extern/box2d/src/core.h"

#if !defined( NDEBUG ) || defined( MACARON_ENABLE_ASSERT )
extern MacaronAssertFcn* MacaronAssertHandler;
	#define MACARON_ASSERT( condition )                                                                                               \
		do                                                                                                                       \
		{                                                                                                                        \
			if ( !( condition ) && MacaronAssertHandler( #condition, __FILE__, (int)__LINE__ ) )                                      \
				B2_BREAKPOINT;                                                                                                   \
		}                                                                                                                        \
		while ( 0 )
#else
	#define MACARON_ASSERT( ... ) ( (void)0 )
#endif
