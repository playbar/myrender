//=============================================================================
// FILE: svrContainers.h
//
//                  Copyright (c) 2015 QUALCOMM Technologies Inc.
//                              All Rights Reserved.
//
//=============================================================================

#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

namespace Svr
{

// From http://www.cse.yorku.ca/~oz/hash.html
inline unsigned int DjB2Hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while ( (c = *str++) )
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

template <class _Key, class _Data, _Key (*hashFn)(const char*)>
class HashTable
{
private:
	struct Entry
	{
		_Key	key;
		_Data	data;
		Entry*	pNext;
	};

public:
	class Iterator
	{
	friend class HashTable;
		Iterator()
			: mRow(0)
			, mpCurrent(NULL)
		{}

		Iterator(HashTable& source)
			: msource(source)
			, mRow(0)
			, mpCurrent(NULL)
		{}

		_Data& Current()
		{
			return mpCurrent->data;
		}

		void Next()
		{
			if(mpCurrent->pNext != NULL)
			{
				mpCurrent = mpCurrent->pNext;
			}
			else
			{
				while(mRow < msource.mcapacity && msource.mtable[++mRow] == NULL);


				if( mRow == msource.mcapacity )
				{
					mpCurrent = NULL;
				}
				else
				{
					mpCurrent = msource.mtable[mRow];
				}
			}
		}

		bool End()
		{
			return (mRow == msource.mcapacity) && (mpCurrent == NULL);
		}

	private:
		HashTable&	msource;

		unsigned int	mRow;
		Entry*	mpCurrent;
	};

public:
	HashTable()
		: mtable(NULL)
		, mcapacity(0)
	{};

	~HashTable()
	{
		Destroy();
	}

	void Destroy()
	{
		for(unsigned int i=0; i < mcapacity; i++)
		{
			Entry* pEntry = mtable[i];
			while(pEntry)
			{
				Entry* pCurrent = pEntry;
				pEntry = pEntry->pNext;
				delete pCurrent;
			}
			mtable[i] = 0;
		}
		if(mcapacity > 0)
		{
			delete [] mtable;
			mtable = NULL;
			mcapacity = 0;
		}
	}

	void Init(unsigned int capacity)
	{
		mcapacity = capacity;
		mtable = new Entry*[capacity];

		for(unsigned int i=0; i < mcapacity; i++)
			mtable[i] = NULL;
	}

	void Insert(_Key key, _Data& data)
	{
		unsigned int index = key % mcapacity;

		if(mtable[index] == NULL)
		{
			Entry* pEntry = new Entry;
			mtable[index] = pEntry;
			pEntry->key = key;
			pEntry->data = data;
			pEntry->pNext = NULL;
		}
		else
		{
			Entry* pEntry = mtable[index];
			while(pEntry->pNext != NULL)
				pEntry = pEntry->pNext;
			Entry* pNextEntry = new Entry;
			pNextEntry->key = key;
			pNextEntry->data = data;
			pNextEntry->pNext = NULL;
			pEntry->pNext = pNextEntry;
		}
	}

	void Insert(const char* key, _Data& data)
	{
		_Key hashVal = hashFn(key);
		return Insert( hashVal, data );
	}

	bool Find(_Key key, _Data* result)
	{
		unsigned int index = key % mcapacity;

		Entry* pEntry = mtable[index];
		while(pEntry != NULL)
		{
			if(pEntry->key == key)
			{
				*result = pEntry->data;
				return true;
			}
			pEntry = pEntry->pNext;
		}
		return false;
	}

	bool Find(const char* key, _Data* result)
	{
		_Key hashVal = hashFn(key);
		return Find(hashVal, result);
	}

	Iterator GetIterator()
	{
		Iterator res(*this);

		unsigned int i =0;

		while(i < mcapacity)
		{
			if(mtable[i] != NULL)
			{
				res.mpCurrent = mtable[i];
				break;
			}
			i++;
		}
		res.mRow = i;
		return res;
	}

public:
	Entry**	mtable;
	unsigned int 	mcapacity;
};

}
