#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>	
#include <string>
#include "i_process.h"
#include "DynArray.h"
using namespace std;


class i_system
{
	
	
public:
	// Full report specific information
	bool isFullReport;
	
	// The list of i_process information
	DynArray<i_process*> processes;
	i_system(bool fullSnapshot);
	~i_system(void);
	void printIncrementalReport(i_system* oldSystemSnapshot);
	void printFinalReport(i_system* oldSystemSnapshot);

};
