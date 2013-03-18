#ifndef MAX_CPU_BURST
	#define MAX_CPU_BURST 100
#endif

#ifndef _PROCESST_HPP_
#define _PROCESST_HPP_

struct processt {
	int PID;
	int TARQ;
	int PRIO;
	int TNCPU;
	int CPU[MAX_CPU_BURST];
	int IO[MAX_CPU_BURST-1];
	int current_burst; //indicates the current CPU burst, set to 0 if no burst has occured 
	int wait_time;
	int execution_time;
	int turnaround_time;
	int age;
	int average_burst;
	int last_burst;
};

#endif
