#include "Process.cpp"

class Scheduler {

public:
	int nextProcess(vector<Process> processes, Algorithms mode) {
		switch(mode)
		{
			case fifo:
				return fifoScheduler(processes);

			default:
				return 0;
		}
	}

	int fifoScheduler(vector<Process> processes) {
		int index = -1;
		int earliestTarq = 4096;
		for(unsigned int i = 0; i < processes.size(); i++) {
			if(processes[i].getTarq() < earliestTarq && processes[i].isDone() == false) {
				index = i;
				earliestTarq = processes[i].getTarq();
			}
		}

		return index;
	}

private:
	vector<Process> processes;
};

