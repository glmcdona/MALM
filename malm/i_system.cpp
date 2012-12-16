#include "StdAfx.h"
#include "i_system.h"


void i_system::printIncrementalReport(i_system* oldSystemSnapshot)
{
	// Print any closed processes
	for(int i = 0; i < oldSystemSnapshot->processes.GetSize(); i++ )
	{
		// See if this process existed in the previous snapshot
		bool closedProcess = true;
		for( int j = 0; j < this->processes.GetSize(); j++ )
		{
			if( *oldSystemSnapshot->processes[i] ==  *this->processes[j] )
			{
				closedProcess = false;
				break;
			}
		}
		if( closedProcess )
		{
			// Print this as a new process udpate
			printf("PID %X,%S: Terminated.\n",
				oldSystemSnapshot->processes[i]->processDetails.th32ProcessID,
				oldSystemSnapshot->processes[i]->processDetails.szExeFile );
		}
	}

	// Check to see if there are any new processes
	for(int i = 0; i < this->processes.GetSize(); i++ )
	{
		// See if this process existed in the previous snapshot
		bool newProcess = true;
		int j = 0;
		for( j = 0; j < oldSystemSnapshot->processes.GetSize(); j++ )
		{
			if( *oldSystemSnapshot->processes[j] ==  *this->processes[i] )
			{
				newProcess = false;
				break;
			}
		}

		if( newProcess )
		{
			// Print this as a new process udpate
			printf("PID %X,%S: New process.\n",
				this->processes[i]->processDetails.th32ProcessID,
				this->processes[i]->processDetails.szExeFile );
		}else{
			// Check for changes in this process
			this->processes[i]->printIncrementalReport(oldSystemSnapshot->processes[j]);
		}

	}
}

void i_system::printFinalReport(i_system* oldSystemSnapshot)
{
	// Check to see if there are any new processes
	for(int i = 0; i < this->processes.GetSize(); i++ )
	{
		// See if this process existed in the previous snapshot
		bool newProcess = true;
		int j = 0;
		for( j = 0; j < oldSystemSnapshot->processes.GetSize(); j++ )
		{
			if( *oldSystemSnapshot->processes[j] ==  *this->processes[i] )
			{
				newProcess = false;
				break;
			}
		}

		if( newProcess )
		{
			// Print this as a new process udpate
			printf("--- PID %X,%S ---\nNew Process.\n\n",
					this->processes[i]->processDetails.th32ProcessID,
					this->processes[i]->processDetails.szExeFile );
		}else{
			// Check for changes in this process
			this->processes[i]->printFinalReport(oldSystemSnapshot->processes[j]);
		}

	}
}

i_system::i_system(bool fullSnapshot)
{
	// Take an i_system snapshot
	HANDLE hSnapShot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	
	if( hSnapShot != INVALID_HANDLE_VALUE )
	{
		// Handle the first i_process
		PROCESSENTRY32 tmpProcess;
		tmpProcess.dwSize = sizeof(PROCESSENTRY32);
		processes.Clear();
		int result;
		if( (result = Process32First(hSnapShot, &tmpProcess)) )
		{
			// Add this i_process to our array
			if( result == TRUE )
				processes.Add(new i_process(&tmpProcess));

			while( (result = Process32Next(hSnapShot, &tmpProcess)) )
			{
				if( result == TRUE )
					// Add this i_process to our array
					processes.Add(new i_process(&tmpProcess));
			}
		}

		// Cleanup the handle
		CloseHandle( hSnapShot );
	}
}

i_system::~i_system(void)
{
	// Clean up the processes
	for( int i = 0; i < processes.GetSize(); i++ )
	{
		delete processes[i];
	}

	processes.Clear();
}
