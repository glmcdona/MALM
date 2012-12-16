#pragma once
#include "windows.h"
#include <string>
#include "DynArray.h"
#include <iostream>
#include <vector>
using namespace std;
#define BUFSIZE 10000

class i_ext_tool
{
public:
	string output;
	string description;
	i_ext_tool(LPWSTR command, LPCTSTR description);
	~i_ext_tool(void);
	string readOutput(HANDLE g_hChildStd_OUT_Rd);
	string compare( i_ext_tool other );
};
