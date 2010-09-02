/****************************************************************************
Copyright (c) 2010 cocos2d-x.org

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

/** 
 @file
 Based on Chipmunk cpArray.
 ccArray is a faster alternative to NSMutableArray, it does pretty much the
 same thing (stores NSObjects and retains/releases them appropriately). It's
 faster because:
 - it uses a plain C interface so it doesn't incur Objective-c messaging overhead 
 - it assumes you know what you're doing, so it doesn't spend time on safety checks
   (index out of bounds, required capacity etc.)
 - comparisons are done using pointer equality instead of isEqual
 */

#ifndef CC_ARRAY_H
#define CC_ARRAY_H

#include <stdlib.h>
#include <string.h>

#include "NSMutableArray.h"
#include "NSObject.h"

namespace cocos2d {

typedef struct _ccArray 
{
	unsigned int num, max;
	NSObject**    arr; //equals NSObject** arr;
} ccArray;

/** Allocates and initializes a new array with specified capacity */
static inline ccArray* ccArrayNew(unsigned int capacity) 
{
	if (capacity == 0)
	{
		capacity = 1; 
	}
	
	ccArray *arr = (ccArray*)malloc( sizeof(ccArray) );
	arr->num = 0;
	
	arr->arr =  (NSObject**)malloc( capacity * sizeof(NSObject*) );
	arr->max = capacity;
	
	return arr;
}

static inline void ccArrayRemoveAllObjects(ccArray *arr);

/** Frees array after removing all remaining objects. Silently ignores nil arr. */
static inline void ccArrayFree(ccArray *arr)
{
	if( arr == NULL ) 
	{
		return;
	}
	
	ccArrayRemoveAllObjects(arr);
	
	//delete arr->m_pObjectArray; 
	free(arr->arr);
	
	free(arr);
}

/** Doubles array capacity */
static inline void ccArrayDoubleCapacity(ccArray *arr)
{
	arr->max *= 2;
	arr->arr = (NSObject**) realloc(arr->arr, arr->max * sizeof(NSObject*));
}

/** Increases array capacity such that max >= num + extra. */
static inline void ccArrayEnsureExtraCapacity(ccArray *arr, unsigned int extra)
{
	while (arr->max < arr->num + extra)
	{
		ccArrayDoubleCapacity(arr); 
	}
}

/** Returns index of first occurence of object, UXNotFound if object not found. */
static inline int ccArrayGetIndexOfObject(ccArray *arr, NSObject* object)
{
	for ( unsigned int i = 0; i < arr->num; i++)
	{
		if (arr->arr[i] == object) 
		{
			return i;
		}
	}

	return -1; 
}

/** Returns a Boolean value that indicates whether object is present in array. */
static inline bool ccArrayContainsObject(ccArray *arr, NSObject* object)
{
	return ccArrayGetIndexOfObject(arr, object) != -1;
}

/** Appends an object. Bahaviour undefined if array doesn't have enough capacity. */
static inline void ccArrayAppendObject(ccArray *arr, NSObject* object)
{
	arr->arr[arr->num] = object; object->retain();
	arr->num++;
}

/** Appends an object. Capacity of arr is increased if needed. */
static inline void ccArrayAppendObjectWithResize(ccArray *arr, NSObject* object)
{
	ccArrayEnsureExtraCapacity(arr, 1);
	ccArrayAppendObject(arr, object);
}

/** Appends objects from plusArr to arr. Behaviour undefined if arr doesn't have
 enough capacity. */
static inline void ccArrayAppendArray(ccArray *arr, ccArray *plusArr)
{
	for( unsigned int i = 0; i < plusArr->num; i++)
	{
		ccArrayAppendObject(arr, plusArr->arr[i]); 
	}
}

/** Appends objects from plusArr to arr. Capacity of arr is increased if needed. */
static inline void ccArrayAppendArrayWithResize(ccArray *arr, ccArray *plusArr)
{
	ccArrayEnsureExtraCapacity(arr, plusArr->num); 
	ccArrayAppendArray(arr, plusArr);
}

/** Removes all objects from arr */
static inline void ccArrayRemoveAllObjects(ccArray *arr)
{
	while(arr->num > 0)
	{
		arr->arr[--arr->num]->release();
	}
}

/** Removes object at specified index and pushes back all subsequent objects.
 Behaviour undefined if index outside [0, num-1]. */
static inline void ccArrayRemoveObjectAtIndex(ccArray *arr, unsigned int index)
{
	arr->arr[index]->release(); 
	
	for( unsigned int last = --arr->num; index < last; index++)
	{
		arr->arr[index] = arr->arr[index + 1]; 
	}
}

/** Removes object at specified index and fills the gap with the last object,
 thereby avoiding the need to push back subsequent objects.
 Behaviour undefined if index outside [0, num-1]. */
static inline void ccArrayFastRemoveObjectAtIndex(ccArray *arr, unsigned int index)
{
	arr->arr[index]->release();

	unsigned int last = --arr->num;
	arr->arr[index] = arr->arr[last];
}

/** Searches for the first occurance of object and removes it. If object is not
 found the function has no effect. */
static inline void ccArrayRemoveObject(ccArray *arr, NSObject* object)
{
	unsigned int index = ccArrayGetIndexOfObject(arr, object);

	if (index != -1)
	{
		ccArrayRemoveObjectAtIndex(arr, index);
	}
}

/** Removes from arr all objects in minusArr. For each object in minusArr, the
 first matching instance in arr will be removed. */
static inline void ccArrayRemoveArray(ccArray *arr, ccArray *minusArr)
{
	for( unsigned int i = 0; i < minusArr->num; i++)
	{
		ccArrayRemoveObject(arr, minusArr->arr[i]);
	}
}

/** Removes from arr all objects in minusArr. For each object in minusArr, all
 matching instances in arr will be removed. */
static inline void ccArrayFullRemoveArray(ccArray *arr, ccArray *minusArr)
{
	unsigned int back = 0;
	
	for( unsigned int i = 0; i < arr->num; i++) 
	{
		if( ccArrayContainsObject(minusArr, arr->arr[i]) ) 
		{
			delete arr->arr[i]; 
			back++;
		}
		else
		{
			arr->arr[i - back] = arr->arr[i];
		}
	}
	
	arr->num -= back;
}

typedef struct _ccCArray 
{
	unsigned int num, max;
	void**    arr; //equals NSObject** arr;
} ccCArray;

static inline void ccCArrayRemoveAllValues(ccCArray *arr);

/** Allocates and initializes a new C array with specified capacity */
static inline ccCArray* ccCArrayNew(unsigned int capacity) 
{
	if (capacity == 0)
	{
		capacity = 1; 
	}
	
	ccCArray *arr = (ccCArray*)malloc( sizeof(ccCArray) );
	arr->num = 0;
	arr->arr =  (void**) malloc( capacity * sizeof(void*) );
	arr->max = capacity;
	
	return arr;
}

/** Frees C array after removing all remaining values. Silently ignores nil arr. */
static inline void ccCArrayFree(ccCArray *arr)
{
	if( arr == NULL ) 
	{
		return;
	}
	
	ccCArrayRemoveAllValues(arr);
	
	free(arr->arr);
	free(arr);
}

/** Doubles C array capacity */
static inline void ccCArrayDoubleCapacity(ccCArray *arr)
{
	arr->max *= 2;
	arr->arr = (void**) realloc(arr->arr, arr->max * sizeof(void*));
}

/** Increases array capacity such that max >= num + extra. */
static inline void ccCArrayEnsureExtraCapacity(ccCArray *arr, unsigned int extra)
{
	while (arr->max < arr->num + extra)
	{
		ccCArrayDoubleCapacity(arr); 
	}
}

/** Returns index of first occurence of value, NSNotFound if value not found. */
static inline int ccCArrayGetIndexOfValue(ccCArray *arr, void* value)
{
	for (unsigned int i = 0; i < arr->num; i++)
	{
		if (arr->arr[i] == value) 
		{
			return i;
		}
	}

	return -1;
}

/** Returns a Boolean value that indicates whether value is present in the C array. */
static inline bool ccCArrayContainsValue(ccCArray *arr, void* value)
{
	return ccCArrayGetIndexOfValue(arr, value) != -1;
}

/** Inserts a value at a certain position. The valid index is [0, num] */
static inline void ccCArrayInsertValueAtIndex( ccCArray *arr, void* value, unsigned int index)
{
	int remaining = arr->num - index;

	// make sure it has enough capacity
	if (arr->num + 1 == arr->max)
	{
        ccCArrayDoubleCapacity(arr);
	}
	
	// last Value doesn't need to be moved
	if( remaining > 0) 
	{
		// tex coordinates
		memmove( &arr->arr[index+1],&arr->arr[index], sizeof(void*) * remaining );
	}
	
	arr->num++;	
	arr->arr[index] = value;
}

/** Appends an value */
static inline void ccCArrayAppendValue(ccCArray *arr, void* value)
{
	arr->arr[arr->num] =  value;
	arr->num++;

	// double the capacity for the next append action
	// if the num >= max
	if (arr->num >= arr->max)
	{
		ccCArrayDoubleCapacity(arr);
	}
}

/** Appends an value. Capacity of arr is increased if needed. */
static inline void ccCArrayAppendValueWithResize(ccCArray *arr, void* value)
{
	ccCArrayEnsureExtraCapacity(arr, 1);
	ccCArrayAppendValue(arr, value);
}

/** Appends values from plusArr to arr. Behaviour undefined if arr doesn't have
 enough capacity. */
static inline void ccCArrayAppendArray(ccCArray *arr, ccCArray *plusArr)
{
	for (unsigned int i = 0; i < plusArr->num; i++)
	{
		ccCArrayAppendValue(arr, plusArr->arr[i]);
	}
}

/** Appends values from plusArr to arr. Capacity of arr is increased if needed. */
static inline void ccCArrayAppendArrayWithResize(ccCArray *arr, ccCArray *plusArr)
{
	ccCArrayEnsureExtraCapacity(arr, plusArr->num);
	ccCArrayAppendArray(arr, plusArr);
}

/** Removes all values from arr */
static inline void ccCArrayRemoveAllValues(ccCArray *arr)
{
	arr->num = 0;
}

/** Removes value at specified index and pushes back all subsequent values.
 Behaviour undefined if index outside [0, num-1]. */
static inline void ccCArrayRemoveValueAtIndex(ccCArray *arr, unsigned int index)
{	
	for (unsigned int last = --arr->num; index < last; index++)
	{
		arr->arr[index] = arr->arr[index + 1];
	}
}

/** Removes value at specified index and fills the gap with the last value,
 thereby avoiding the need to push back subsequent values.
 Behaviour undefined if index outside [0, num-1]. */
static inline void ccCArrayFastRemoveValueAtIndex(ccCArray *arr, unsigned int index)
{
	unsigned int last = --arr->num;
	arr->arr[index] = arr->arr[last];
}

/** Searches for the first occurance of value and removes it. If value is not
 found the function has no effect. */
static inline void ccCArrayRemoveValue(ccCArray *arr, void* value)
{
	unsigned int index = ccCArrayGetIndexOfValue(arr, value);
	if (index != -1)
	{
		ccCArrayRemoveValueAtIndex(arr, index);
	}
}

/** Removes from arr all values in minusArr. For each Value in minusArr, the
 first matching instance in arr will be removed. */
static inline void ccCArrayRemoveArray(ccCArray *arr, ccCArray *minusArr)
{
	for( unsigned int i = 0; i < minusArr->num; i++)
	{
		ccCArrayRemoveValue(arr, minusArr->arr[i]);
	}
}

/** Removes from arr all values in minusArr. For each value in minusArr, all
 matching instances in arr will be removed. */
static inline void ccCArrayFullRemoveArray(ccCArray *arr, ccCArray *minusArr)
{
	unsigned int back = 0;
	
	for (unsigned int i = 0; i < arr->num; i++) 
	{
		if( ccCArrayContainsValue(minusArr, arr->arr[i]) ) 
		{
			back++;
		} else
		{
			arr->arr[i - back] = arr->arr[i];
		}
	}
	
	arr->num -= back;
} 

}

#endif // CC_ARRAY_H
