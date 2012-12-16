#include "StdAfx.h"
#include "i_heap.h"

bool operator== (i_heap &heap1, i_heap &heap2)
{
	return (heap1.heapDetails.AllocationBase == heap2.heapDetails.AllocationBase)
		&&  (heap1.heapDetails.RegionSize == heap2.heapDetails.RegionSize);
}

i_heap::i_heap(MEMORY_BASIC_INFORMATION details)
{
	this->heapDetails = details;
}

i_heap::~i_heap(void)
{
}
