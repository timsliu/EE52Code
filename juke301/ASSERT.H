/* assert.h - Defines assert macro. 
** Copyright (c) 1982, 1983, 1984, 1985 by Mark Williams Company, Chicago.
** All rights reserved. May not be copied or disclosed without permission.
*/

#if NDEBUG
#define	assert(p)
#else
#define	assert(p)	if(!(p)){printf("%s: %d: assert(%s) failed.\n",\
			    __FILE__, __LINE__, #p);exit(1);}
#endif
