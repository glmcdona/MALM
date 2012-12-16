#pragma once
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>	
#include <string>
#include "DynArray.h"

class i_module
{
public:
	MODULEENTRY32W moduleDetails;
	i_module(MODULEENTRY32W details);
	~i_module(void);
	bool operator== (const i_module &other) const;
};
