#pragma once

#define _DEBUG

#ifdef _DEBUG
#define DEBUG( x ) printf( x )
#elif
#define DEBUG( x )
#endif	
