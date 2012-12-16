#include "StdAfx.h"
#include "i_ext_tool.h"

string i_ext_tool::compare( i_ext_tool other )
{
	// Compare and print the difference
	return "";
}

string i_ext_tool::readOutput(HANDLE g_hChildStd_OUT_Rd)
{
	DWORD dwRead, dwWritten; 
	CHAR chBuf[BUFSIZE]; 
	BOOL bSuccess = FALSE;
	string result = "";
	for (;;) 
	{ 
		bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if( ! bSuccess || dwRead == 0 ) break; 
		result = result.append( chBuf );
	} 
	return "";
}

i_ext_tool::i_ext_tool(LPWSTR command, LPCTSTR description)
{
	PROCESS_INFORMATION piProcInfo; 
	STARTUPINFO siStartInfo;
	BOOL bSuccess = FALSE;
	SECURITY_ATTRIBUTES saAttr; 
	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;
	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;
		
	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL; 

	// Create a pipe for the child process's STDOUT. 
	if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
		printf("Failed to create pipe for %S.", command);

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
		printf("Failed to set handle information for %S.", command);
 
	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
 
	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO); 
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
 
	// Create the child process. 
	bSuccess = CreateProcess(NULL, 
		command,	  // command line 
		NULL,			 // process security attributes 
		NULL,			 // primary thread security attributes 
		TRUE,			 // handles are inherited 
		0,				 // creation flags 
		NULL,			 // use parent's environment 
		NULL,			 // use parent's current directory 
		&siStartInfo,  // STARTUPINFO pointer 
		&piProcInfo);  // receives PROCESS_INFORMATION 
	
	// If an error occurs, exit the application. 
	if ( ! bSuccess ) 
		// Failed
		printf("Failed to launch %S.", command);
	else 
	{
		// Wait until the child processes closes
		WaitForSingleObject(piProcInfo.hProcess, INFINITE);

		// Read the output from the process
		output = this->readOutput(g_hChildStd_OUT_Rd);

		cout << output;

		CloseHandle(piProcInfo.hProcess);
		CloseHandle(piProcInfo.hThread);
	}
}

i_ext_tool::~i_ext_tool(void)
{
}
