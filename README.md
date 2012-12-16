# MALM: Malware Monitor
MALM is a Windows x86 and x64 compatible tool that records new processes, new modules loaded by existing processes, and new executable heaps in existing processes. Run this prior to running the malware sample. malm will log changes it has found, and upon closing (CTRL-C) this tool will print a final report of the state change from the beginning to the end. This tool is quite useful for monitoring where malware resides after execution. This tool is based upon snapshots,so it can miss processes, modules, or heaps that exist for only a short period of time.

I am maintaining a public binary release download page for this project at:
  http://split-code.com/index.php/malm-malware-monitor


## Flags
The command-line flags for MALM are as follows:

   -q: quick mode.
        Only generates final report, instead of continually taking snapshots and printing the incremental reports.

   -t [seconds]: time limit.
        Quit and generate final report after the specified number of

		
## Example Usage
1. Run cmd.exe as Administrator.
2. In cmd.exe, run MALM.
3. Execute the malware sample to monitor.
4. Wait for the malware sample to infect your system. MALM will be printing the incremental reports in cmd.exe.
5. In cmd.exe, press CTRL-C. The final report will be printed at this time.	


## Example Output
The following is a recording when running a live malware sample that allocates executable heaps in the existing svchost.exe process, and copies itself into it. The final report was triggered by a CTRL-C keyboard command. Take appropriate precautions when handling computer viruses - this tool simply monitors the system and does not prevent infection.

	PID 690,7.exe: New process.
	PID 690,7.exe: No longer accessible from current process security token.
	PID 690,7.exe: Terminated.
	PID 3DC,svchost.exe: New executable heap at 0x7A0000
	PID 3DC,svchost.exe: New executable heap at 0x7A1000
	PID 3DC,svchost.exe: New executable heap at 0x7A3000
	PID 3DC,svchost.exe: New executable heap at 0x7A6000
	PID 3DC,svchost.exe: New executable heap at 0x7A8000
	PID 3DC,svchost.exe: New executable heap at 0x7B2000
	PID 3DC,svchost.exe: New executable heap at 0x7B3000
	PID 3DC,svchost.exe: New executable heap at 0x7B6000
	
	Final report of final state versus starting state.
	--- PID 3DC,svchost.exe ---
	new exec heap: 7A0000
	new exec heap: 7A1000
	new exec heap: 7A3000
	new exec heap: 7A6000
	new exec heap: 7A8000
	new exec heap: 7B2000
	new exec heap: 7B3000
	new exec heap: 7B6000


## Contributing
Contributions are welcome. Some possible contribution directions are as follows:
* Upgrade the i_module comparer to return 'False' when the code at the module entry point has changed, or if the PE header has changed. Some malware gut out existing modules in memory and replace the code with their malware. In this scenario, this tool may not register the hidden location of the malware within the legitimate previously-loaded module.
* Kernel address-space executable region monitoring.
* Maybe add filesystem and registry change monitoring with a flag.
* Anything else you can think of.


## License
Copyright 2012 Geoff McDonald, and other contributors.
http://split-code.com/

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


