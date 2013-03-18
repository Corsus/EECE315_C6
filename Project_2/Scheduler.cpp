#include "Process.cpp"

class Scheduler {

public:

	void setTimeQuantum(int timeQuantum) {
		this->timeQuantum = timeQuantum;
	}

	int nextProcess(vector<Process> processes, Algorithms mode) {
		switch(mode) {
			case fifo:
				return fifoAlgorithm(processes);
			case rr:
				return rrAlgorithm(processes);
			case prio:
				return prioAlgorithm(processes);
			case prePrio:
				return prePrioAlgorithm(processes);
			case timeslicePrio:
				return timeslicePrioAlgorithm(processes);
			case sjf:
				return sjfAlgorithm(processes);
			case spb:
				return spbAlgorithm(processes);
			default:
				return 0;
		}
	}

	int fifoAlgorithm(vector<Process> processes) {
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

	int rrAlgorithm(vector<Process> processes) {
	}

	int prioAlgorithm(vector<Process> processes) {
	}

	int prePrioAlgorithm(vector<Process> processes) {
	}

	int timeslicePrioAlgorithm(vector<Process> processes) {
	}

	int sjfAlgorithm(vector<Process> processes) {
	}

	int spbAlgorithm(vector<Process> processes) {
	}
private:
	int timeQuantum;
	vector<Process> processes;
};

