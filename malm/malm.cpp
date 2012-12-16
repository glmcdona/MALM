// malm.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "i_system.h"
#include <signal.h>
#include <cstdlib>
#include <stdio.h>
#include <time.h>
using namespace std;

i_system* startSnapshot;

BOOL Is64BitWindows()
{
	#if defined(_WIN64)
		return TRUE;  // 64-bit programs run only on Win64
	#elif defined(_WIN32)
		// 32-bit programs run on both 32-bit and 64-bit Windows
		// so must sniff
		BOOL f64 = FALSE;
		return IsWow64Process(GetCurrentProcess(), &f64) && f64;
	#else
		return FALSE; // Win64 does not support Win16
	#endif
}

bool isElevated(HANDLE h_Process)
{
	HANDLE h_Token;
	TOKEN_ELEVATION t_TokenElevation;
    TOKEN_ELEVATION_TYPE e_ElevationType;
	DWORD dw_TokenLength;
	
	if( OpenProcessToken(h_Process, TOKEN_READ | TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES , &h_Token) )
	{
		if(GetTokenInformation(h_Token,TokenElevation,&t_TokenElevation,sizeof(t_TokenElevation),&dw_TokenLength))
		{
			if(t_TokenElevation.TokenIsElevated != 0)
			{
				if(GetTokenInformation(h_Token,TokenElevationType,&e_ElevationType,sizeof(e_ElevationType),&dw_TokenLength))
				{
					if(e_ElevationType == TokenElevationTypeFull || e_ElevationType == TokenElevationTypeDefault)
					{
						return true;
					}
				}
			}
		}
	}

    return false;
}

bool getMaximumPrivileges(HANDLE h_Process)
{
	HANDLE h_Token;
	DWORD dw_TokenLength;
	if( OpenProcessToken(h_Process, TOKEN_READ | TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES , &h_Token) )
	{
		// Read the old token privileges
		TOKEN_PRIVILEGES* privilages = new TOKEN_PRIVILEGES[100];
		if( GetTokenInformation(h_Token, TokenPrivileges, privilages,sizeof(TOKEN_PRIVILEGES)*100,&dw_TokenLength) )
		{
			// Enable all privileges
			for( int i = 0; i < privilages->PrivilegeCount; i++ )
			{
				privilages->Privileges[i].Attributes = SE_PRIVILEGE_ENABLED;
			}
			
			// Adjust the privilges
			if(AdjustTokenPrivileges( h_Token, false, privilages, sizeof(TOKEN_PRIVILEGES)*100, NULL, NULL  ))
			{
				delete[] privilages;
				return true;
			}
		}
		delete[] privilages;
	}
	return false;
}

void printFinalReport()
{
	// We need to print the final report on the changes
	printf("\n\nFinal report of final state versus starting state:\n");

	// Take a new snapshot
	i_system* currentSnapshot = new i_system(true);
	
	// Print the final report
	currentSnapshot->printFinalReport( startSnapshot );
}

BOOL WINAPI ConsoleHandler(DWORD CEvent)
{
    char mesg[128];

    switch(CEvent)
    {
    case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
		printFinalReport();
		exit(0);
        break;

    }
    return TRUE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Disable buffering
	setbuf(stdout, NULL);

	// Print the intro
	printf("MALM v1.2 Flags\n\
   -q: quick mode. Only generates final report.\n\
   -t [seconds]: Quit and generate final report after the specified number of seconds.\n\n\
   \
Welcome to malm v1.2: malware monitor\n\
\tby Geoff McDonald\n\
\thttp://www.split-code.com/\n\n\
This is a Windows x86 and x64 compatible tool that records new processes, new modules loaded by existing processes, and new executable heaps in existing processes. Run this prior to running the malware sample. malm will log changes it has found, and upon closing (CTRL-C) this tool will print a final report of the state change from the beginning to the end. This tool is quite useful for monitoring where malware is residing after execution. This tool is based upon snapshots, so it can miss processes, modules, or heaps that exist for only a short period of time.\n\n");

	// Read in the command-line arguments
	int numSeconds = 0;
	bool quickMode = false;
	for( int i = 0; i < argc; i++ )
	{
		if( wcscmp(argv[i],L"-q") == 0 )
			quickMode = true;
		if( wcscmp(argv[i],L"-t") == 0 && i < argc - 1 )
		{
			// Parse the number of seconds to quit after
			numSeconds = _wtoi(argv[i+1]); // Returns 0 if it cannot parse it. No problem here.
		}
	}
	if( quickMode )
		printf("Quick mode enabled.\n\n");
	if( numSeconds > 0 )
		printf("Automatically quitting and generating final report in %i seconds.\n\n", numSeconds);

	// Warn if the process was not run as administrator
	HANDLE h_Process = GetCurrentProcess();
	if( !isElevated(h_Process) )
	{
		printf("WARNING: This tool should be run with administrator rights to properly monitor all processes.\n\n");
	}

	// Request maximum thread token privileges
	if( !getMaximumPrivileges(h_Process) )
	{
		printf("WARNING: Failed to adjust token privileges. This will likely result in missing the monitoring of some processes due to insufficient privileges.\n\n");
	}

	// Warn if running in 32 bit mode on a 64 bit OS
	if( Is64BitWindows() && sizeof(void*) == 4 )
	{
		printf("WARNING: To properly monitor all processes on a 64 bit Windows version, the malm64 version of this tool should be used. Currently malm is running as a 32bit process under a 64bit operating system.\n\n");
	}
	
	// Register the quit handler
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler,TRUE)==FALSE)
	{
		// unable to install handler... 
		// display message to the user
		printf("WARNING: Unable to install keyboard handler. This means that the final report will not be generated upon exiting.\n");
	}

	// Take an initial i_system snapshot
	startSnapshot = new i_system(true);
	i_system* lastSnapshot = new i_system(false);

	// Begin monitoring for changes
	printf("Recording changes...\n\n");
	time_t startTime = time (NULL);
	while( true )
	{

		if( !quickMode )
		{
			i_system* currentSnapshot = new i_system(false);

			// Print changes
			currentSnapshot->printIncrementalReport(lastSnapshot);

			// Delete and replace last snapshot
			delete lastSnapshot;
			lastSnapshot = currentSnapshot;
		}
		Sleep(10);

		if( numSeconds > 0 )
		{
			// Check to see if we are out of time
			if( time(NULL) - startTime > numSeconds )
			{
				// Finished, generate final report and quit
				printFinalReport();
				return 0;
			}
		}
	}
	return 0;
}

