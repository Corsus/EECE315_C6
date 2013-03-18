#include "Process.cpp"
#include "Scheduler.cpp"


class Simulator {
public:
	Simulator () {
		allProcessesComplete = false;
	}

	void set(vector<Process> processes, Algorithms mode) {
		masterProcessList = processes;
		this->mode = mode;
	}

	int tick() {
		int index = scheduler.nextProcess(masterProcessList, mode);

		//-1 signals completion of all processes 
		if (index != -1) {
			masterProcessList[index].process();
			ageAllProcesses();
			return masterProcessList[index].getPid();
		}
		else {
			allProcessesComplete = true;
		}

		return index;
	}

	vector<Process> getProcesses() {
		return masterProcessList;
	}

	bool isComplete() {
		return allProcessesComplete;
	}

private:
	vector<Process> masterProcessList;
	Scheduler scheduler;
	Algorithms mode;
	bool allProcessesComplete;

	void ageAllProcesses() {
		for(unsigned int i = 0; i < masterProcessList.size(); i++) {
			masterProcessList[i].age();
		}
	}
};