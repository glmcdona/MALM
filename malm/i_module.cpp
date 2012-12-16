#include "StdAfx.h"
#include "i_module.h"

bool i_module::operator== (const i_module &other) const
{
	return this->moduleDetails.hModule == other.moduleDetails.hModule;
}

i_module::i_module(MODULEENTRY32W details)
{
	moduleDetails = details;
}

i_module::~i_module(void)
{
}
