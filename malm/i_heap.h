#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>	
#include <string>
#include "DynArray.h"

class i_heap
{
public:
	MEMORY_BASIC_INFORMATION heapDetails;
	i_heap(MEMORY_BASIC_INFORMATION details);
	~i_heap(void);

	friend bool operator== (i_heap &heap1, i_heap &heap2);
};
