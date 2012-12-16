#include "StdAfx.h"
#include "i_process.h"

bool IsWin64(HANDLE process)
{
    BOOL retVal;
    return IsWow64Process(process, &retVal) && retVal;
}

bool i_process::operator== (const i_process &other) const
{
	return (this->processDetails.th32ProcessID == other.processDetails.th32ProcessID);
}

void i_process::printIncrementalReport(i_process* oldProcessSnapshot)
{
	if( !(this->failed || oldProcessSnapshot->failed) )
	{
		// Print any new executable heaps or new loaded modules.
		
		// Print new modules
		for(int i = 0; i < this->modules.GetSize(); i++ )
		{
			// See if this executable heap existed in the previous snapshot
			bool changedOrNew = true;
			for( int j = 0; j < oldProcessSnapshot->modules.GetSize(); j++ )
			{
				if( *oldProcessSnapshot->modules[j] ==  *this->modules[i] )
				{
					changedOrNew = false;
					break;
				}
			}

			// Print this executable heap if it changed
			if( changedOrNew )
			{
				printf("PID %X,%S: Loaded module %S at 0x%X\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile,
					this->modules[i]->moduleDetails.szModule,
					this->modules[i]->moduleDetails.hModule );
			}
		}

		// Print unloaded modules
		for( int j = 0; j < oldProcessSnapshot->modules.GetSize(); j++ )
		{
			// See if this module exists only in the old snapshot
			bool existsOnlyInOld = true;
			for(int i = 0; i < this->modules.GetSize(); i++ )
			{
				if( *oldProcessSnapshot->modules[j] ==  *this->modules[i] )
				{
					existsOnlyInOld = false;
					break;
				}
			}

			// Print this module, since it has been unloaded
			if( existsOnlyInOld )
			{
				printf("PID %X,%S: Unloaded module %S\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile,
					oldProcessSnapshot->modules[j]->moduleDetails.szModule );
			}
		}

		// Check to see if there are any new executable heaps
		for(int i = 0; i < this->heaps.GetSize(); i++ )
		{
			// See if this executable heap existed in the previous snapshot
			bool changedOrNew = true;
			for( int j = 0; j < oldProcessSnapshot->heaps.GetSize(); j++ )
			{
				if( *oldProcessSnapshot->heaps[j] ==  *this->heaps[i] )
				{
					changedOrNew = false;
					break;
				}
			}

			// Print this executable heap if it changed
			if( changedOrNew )
			{
				printf("PID %X,%S: New executable heap at 0x%X\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile,
					this->heaps[i]->heapDetails.BaseAddress );
			}
		}
	}else{
		// Accessing at least of the processes failed
		if( this->failed && !oldProcessSnapshot->failed )
		{
			// The process is no longer accessible from our security level
			printf("PID %X,%S: No longer accessible from current process security token.\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile );
		}else if( !this->failed && oldProcessSnapshot->failed )
		{
			// The process is now accessible, but was not previously
			printf("PID %X,%S: Has now become accesible from current process security token.\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile );
		}
	}
}

void i_process::printFinalReport(i_process* oldProcessSnapshot)
{
	if( !(this->failed || oldProcessSnapshot->failed) )
	{
		// Print any new executable heaps or new loaded modules.
		char* changedString = new char[0x3000];
		changedString[0] = 0;
		
		// Print new modules
		for(int i = 0; i < this->modules.GetSize(); i++ )
		{
			// See if this executable heap existed in the previous snapshot
			bool changedOrNew = true;
			for( int j = 0; j < oldProcessSnapshot->modules.GetSize(); j++ )
			{
				if( *oldProcessSnapshot->modules[j] ==  *this->modules[i] )
				{
					changedOrNew = false;
					break;
				}
			}

			// Print this module if it changed
			if( changedOrNew )
			{
				if( strlen( changedString ) < 0x2000 )
					sprintf( changedString, "%sloaded module %S at %X\n", changedString, this->modules[i]->moduleDetails.szModule, this->modules[i]->moduleDetails.hModule);
			}
		}

		

		// Check to see if there are any new executable heaps
		for(int i = 0; i < this->heaps.GetSize(); i++ )
		{
			// See if this executable heap existed in the previous snapshot
			bool changedOrNew = true;
			for( int j = 0; j < oldProcessSnapshot->heaps.GetSize(); j++ )
			{
				if( *oldProcessSnapshot->heaps[j] ==  *this->heaps[i] )
				{
					changedOrNew = false;
					break;
				}
			}

			// Print this executable heap if it changed
			if( changedOrNew )
			{
				if( strlen( changedString ) < 0x2000 )
					sprintf( changedString, "%snew exec heap: %X\n", changedString, this->heaps[i]->heapDetails.BaseAddress);
			}
		}

		
		if( strlen( changedString ) > 0 )
			// Print the results
			printf("--- PID %X,%S ---\n%s\n\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile,
					changedString);
	}else{
		// Accessing at least of the processes failed
		if( this->failed && !oldProcessSnapshot->failed )
		{
			// The process is no longer accessible from our security level
			printf("--- PID %X,%S ---\nNo longer accessible from current process security token.\n\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile );
		}else if( !this->failed && oldProcessSnapshot->failed )
		{
			// The process is now accessible, but was not previously
			printf("--- PID %X,%S ---\nHas now become accesible from current process security token.\n\n",
					this->processDetails.th32ProcessID,
					this->processDetails.szExeFile );
		}
	}
}

i_process::i_process(PROCESSENTRY32* details)
{
	processDetails = *details;

	// Take note of all the PAGE_EXECUTE i_heaps and i_modules.
	
	// Take a i_process snapshot
	HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, details->th32ProcessID);
	if ( hSnapshot == INVALID_HANDLE_VALUE )
	{
		//printf ("WARNING: Could not gather process information for process \"%S\" pid 0x%X, error code (%d).\n", details->szExeFile, details->th32ProcessID, GetLastError());
		failed = true;
		return;
	}
	failed = false;

	// Generate i_module list
	this->generateModuleList(hSnapshot);

	// Open the process to generate a heap map
	HANDLE ph = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, details->th32ProcessID);
	if( ph != NULL )
	{
		// Generate i_heap list
		this->generateHeapList(ph);
	}else{
		failed = true;
	}

	// Cleanup
	CloseHandle( hSnapshot );
}

void i_process::generateModuleList(HANDLE hSnapshot)
{
	MODULEENTRY32 tmpModule;
	tmpModule.dwSize = sizeof(MODULEENTRY32);
	if( Module32First(hSnapshot, &tmpModule) )
	{
		// Add this i_module to our array
		tmpModule.dwSize = sizeof(MODULEENTRY32);
		modules.Add(new i_module(tmpModule));

		while(Module32Next(hSnapshot,&tmpModule))
		{
			// Add this i_module to our array
			modules.Add(new i_module(tmpModule));
			tmpModule.dwSize = sizeof(MODULEENTRY32);
		}
	}
}

void i_process::generateHeapList(HANDLE ph)
{
	// Set the max address of the target process
	__int64 maxAddress = 0;
	if( IsWin64(ph) )
	{
		maxAddress = 0x7fffffffffffffff;
	}else
	{
		maxAddress = 0x7fffffff;
	}

    // Walk the process heaps
    __int64 address = 0;
    MEMORY_BASIC_INFORMATION mbi;
	
    while (address < maxAddress)
    {
        // Load this heap information
        __int64 blockSize = VirtualQueryEx(ph, (LPCVOID) address, &mbi, sizeof(MEMORY_BASIC_INFORMATION));
		__int64 newAddress = (__int64)mbi.BaseAddress + (__int64)mbi.RegionSize + 1;
		if( newAddress <= address )
			break;
		address = newAddress;

        // Add this heap information
		if( (mbi.AllocationProtect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) )
			heaps.Add(new i_heap(mbi));
    }
}

i_process::~i_process(void)
{
	// Clean up the processes
	for( int i = 0; i < heaps.GetSize(); i++ )
	{
		delete heaps[i];
	}
	for( int i = 0; i < modules.GetSize(); i++ )
	{
		delete modules[i];
	}

	heaps.Clear();
	modules.Clear();
}
