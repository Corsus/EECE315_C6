#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "Constants.hpp"

using namespace std;

class Process {
public:
	Process() {
		stringToProcess("0 0 0 0"); //Sets all attributes to 0
	}

	Process(string values) {
		stringToProcess(values);
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
	}

	int getPid() {
		return this->pid;
	}

	int getTarq() {
		return this->tarq;
	}

	int getPrio() {
		return this->prio;
	}

	int getTncpu() {
		return this->tncpu;
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
	
private: 
	//Default attributes
	int pid;
	int tarq;
	int prio;
	int tncpu;
	//Given bursts
	vector<int> cpuBurst;
	vector<int> ioBurst;
	//Attributes to update upon execution
	int currentBurst;
	int waitTime;
	int executionTime;
	int turnaroundTime;
	int age;
	int averageBurst;
	int lastBurst;

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