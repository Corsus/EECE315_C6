#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "Constants.hpp"

#if !defined(PROCESS_CPP_)
#define PROCESS_CPP_

using namespace std;

class Process {
public:
	Process() {
		Process("0 0 0 0"); //Sets all attributes to 0
	}

	Process(string values) {
		stringToProcess(values);
		this->currentCpuBurst = 0;
		this->currentCpuCycle = 0;
		this->currentIOBurst = 0;
		this->currentIOCycle = 0;
		this->processAge = 0;
		this->waitTime = 0;
		this->executionTime = 0;
		this->totalTime = 0;
		this->processing = false;
		this->done = false;
		this->inIO = false;
	}

	//Attaches string of values to this Process
	void stringToProcess(string stringToProcess) {
		vector<int> att;
		
		splitString(stringToProcess, ' ', att);

		//Assign first four attributes
		if (att.size() > 3) {
				pid = att[0];
				tarq = att[1];
				prio = att[2];
				tncpu = att[3];
		}

		//Assign remaining attributes to CPU Bursts and IO Bursts
		for (unsigned int i = PROC_ATTRIBUTES; i < att.size(); i++) {
			cpuBurst.push_back(att[i]);
			i++;
			if (i != att.size()) {
				ioBurst.push_back(att[i]);
			}
		}

		completionTime = 0;
		for(unsigned int i = 0; i < cpuBurst.size(); i++)
			completionTime += cpuBurst[i]; 
	}

	void process() {
		processing = true;
		this->executionTime++;
		this->currentCpuBurst++;

		if(currentCpuCycle < cpuBurst.size() && currentCpuBurst == cpuBurst[currentCpuCycle]) {
			currentCpuCycle++;
			currentCpuBurst = 0;
			inIO = true;
		}

		if(executionTime == completionTime)
			done = true;
	}

	void age() {
		this->totalTime++;
		if (done == false && processing == false) {
			if (inIO == true) {
				currentIOBurst++;
				if(currentIOBurst == ioBurst[currentIOCycle]) {
					inIO = false;
				}
			}
			else {
				this->processAge++;
				this->waitTime++;
			}

		}

		processing = false;
	}

	int getAge() {
		return this->processAge;
	}

	int getPid() {
		return this->pid;
	}

	const int getTarq() {
		return this->tarq;
	}

	int getPrio() {
		return this->prio;
	}

	int getTncpu() {
		return this->tncpu;
	}

	int isDone() {
		return this->done;
	}

	string toString() {
		string stringBuilder = "";
		stringBuilder += " PID: " + to_string(pid);
		stringBuilder += " TARQ: " + to_string(tarq);
		stringBuilder += " PRIO: " + to_string(prio);
		stringBuilder += " TNCPU: " + to_string(tncpu);
		stringBuilder += "\n";
		
		for (unsigned int i = 0; i < cpuBurst.size(); i++) {
			stringBuilder += " CPU Burst " + to_string(i) + ": " + to_string(cpuBurst[i]) + "\n";
			if (i < ioBurst.size())
				stringBuilder += "  IO Burst " + to_string(i) + ": " + to_string(ioBurst[i]) + "\n"; 
		}

		return stringBuilder;
	}

	string statusString() {
		string stringBuilder = "";
		stringBuilder += " PID: " + to_string(pid);
		stringBuilder += " Time Executed: " + to_string(executionTime);
		stringBuilder += " Completion Time: " + to_string(completionTime);
		stringBuilder += " Process Age: " + to_string(processAge);
		stringBuilder += " Done: " + to_string(done);
		stringBuilder += " TARQ: " + to_string(tarq);
		stringBuilder += "\n";

		return stringBuilder;
	}

private: 
	//Default attributes
	int pid;
	int tarq;
	int prio;
	int tncpu;
	//Given bursts
	int currentCpuBurst;
	int currentCpuCycle;
	vector<int> cpuBurst;
	int currentIOBurst;
	int currentIOCycle;
	vector<int> ioBurst;
	//Attributes to update upon execution
	int executionTime;
	int completionTime;

	int waitTime;
	
	int totalTime;
	int processAge;
	int averageBurst;
	int lastBurst;

	bool processing;
	bool done;
	bool inIO;

	vector<int> &splitString(const string &s, char delim, vector<int> &elems) 
	{
		stringstream ss(s+' ');
		string item;
		while(getline(ss, item, delim)) {
			elems.push_back(atoi(item.c_str()));
		}
		return elems;
	}
};

#endif
