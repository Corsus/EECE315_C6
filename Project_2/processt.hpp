#ifndef _PROCESST_HPP_
#define _PROCESST_HPP_

struct processt {
	int PID;
	int TARQ;
	int PRIO;
	int TNCPU;
	int CPU[100];
	int IO[99];
	int current_burst; //indicates the current CPU burst, set to 0 if no burst has occured 
};

#endif
