#if !defined(CONSTANTS_HPP_)
#define CONSTANTS_HPP_

//Definitions
#define MAX_CPU_BURST 100
#define MAX_ATTRIBUTES MAX_CPU_BURST * 2 + PROC_ATTRIBUTES
#define MAX_PROCESS 100
 //PID TARQ PRIO TNCPU
#define PROC_ATTRIBUTES 4

//Settings
#define DEFAULT_FILE_PATH "c:/example.txt"

//Debug Switches
#define DEBUG_PROCESS
#define DEBUG_PROCESSFILE
#define DEBUG_SIMULATOR

enum Algorithms {
	fifo,
	rr,
	prio,
	prePrio,
	timeslicePrio,
	sjf,
	spb
};

#endif