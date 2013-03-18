#include <iostream>
#include <string>
#include "Process.cpp"
#include "ProcessFile.cpp"
#include "Simulator.cpp"
#include <vector>

using namespace std;

void main() {
	vector<Process> Processes;
	ProcessFile fileHandler;
	Simulator sim;
	Algorithms mode;

	fileHandler.setPath(DEFAULT_FILE_PATH);
	while(fileHandler.endOfFile() == false) {
		string line = fileHandler.getLine();
		if(line.size() > PROC_ATTRIBUTES) {
			Process newProcess(line);
			Processes.push_back(newProcess);
		}
	}

	mode = fifo;
	sim.set(Processes, mode);
	
//Tests solely the Process class
#if defined(DEBUG_PROCESS) 
	Process Process1("1 2 3 4 5 6 7 8 9 10 11");
	cout << Process1.toString() << endl;
#endif

//Tests the Processes read from the file handler
#if defined(DEBUG_PROCESSFILE)  
	for(unsigned int i = 0; i < Processes.size(); i++) {
		cout << Processes[i].toString() << endl;
		cout << Processes[i].statusString() << endl;
	}
#endif

//Tests first come first serve, and basic Gantt drawing
#if defined(DEBUG_SIMULATOR)
	while(sim.isComplete() == false)
		cout << to_string(sim.tick());

	cout << endl;
	cout << endl;

	for(unsigned int i = 0; i < Processes.size(); i++) 
		cout << sim.getProcesses()[i].statusString() << endl;
#endif

	cin.get();
}