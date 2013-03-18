#include "Process.cpp"
#include "Scheduler.cpp"

class Simulator {
public:
	Simulator() {
	}

	Simulator(vector<Process> processes, Algorithms mode) {
		masterProcessList = processes;
		this->mode = mode;
	}

	void set(vector<Process> processes, Algorithms mode) {
		masterProcessList = processes;
		this->mode = mode;
	}

	int tick() {
		int index = scheduler.nextProcessScheduled(masterProcessList, mode);
		masterProcessList[index].process();
		ageAllProcesses();
		return masterProcessList[index].getPid();
	}

	vector<Process> getProcesses() {
		return masterProcessList;
	}

private:
	vector<Process> masterProcessList;
	Scheduler scheduler;
	Algorithms mode;

	void ageAllProcesses() {
		for(unsigned int i = 0; i < masterProcessList.size(); i++) {
			masterProcessList[i].age();
		}
	}

};