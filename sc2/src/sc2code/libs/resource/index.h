//Copyright Paul Reiche, Fred Ford. 1992-2002

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef _INDEX_H
#define _INDEX_H

#include "reslib.h"
#include <stdio.h>

typedef struct
{
	RESOURCE res;
	char *res_id;
	void *resdata;
	//COUNT ref;
} ResourceDesc;

typedef struct
{
	ResourceLoadFun *loadFun;
	ResourceFreeFun *freeFun;
} ResourceHandlers;

typedef struct
{
	RES_TYPE numTypes;
			/* Number of types in the handlers array (whether NULL or not).
			 * == the highest stored handler number + 1.
			 */
	ResourceHandlers *handlers;
} ResourceTypeInfo;

struct resource_index
{
	ResourceDesc **res;
	ResourceTypeInfo typeInfo;
	size_t numRes;
};

/* XXX: This should almost certainly be folded into RESOURCE_INDEX wherever possible */
typedef struct resource_index ResourceIndex;

void
forAllResourceIndices(
		void (*callback) (ResourceIndex *idx, void *extra), void *extra);

#endif /* _INDEX_H */

