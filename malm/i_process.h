#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>	
#include <string>
#include "DynArray.h"
#include "i_heap.h"
#include "i_module.h"
using namespace std;

class i_process
{
	DynArray<i_heap*> heaps;
	DynArray<i_module*> modules;
	

	void generateModuleList(HANDLE hSnapshot);
	void generateHeapList(HANDLE hSnapshot);
public:
	bool failed;
	PROCESSENTRY32 processDetails;
	i_process(PROCESSENTRY32* details);
	~i_process(void);
	void printIncrementalReport(i_process* oldProcessSnapshot);
	void printFinalReport(i_process* oldProcessSnapshot);
	bool operator== (const i_process &other) const;
};
