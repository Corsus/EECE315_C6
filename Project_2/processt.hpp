#ifndef _PROCESST_HPP_
#define _PROCESST_HPP_

struct processt {
	int PID;
	int TARQ;
	int PRIO;
	int TNCPU;
	int CPU[100];
	int IO[99];
};

#endif