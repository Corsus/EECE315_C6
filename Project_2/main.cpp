#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define MAX_CPU_BURST 100
//#define MAX_IO_BURST 100
#define MAX_PROCESS 100

struct process {
	int PID;
	int TARQ;
	int PRIO;
	int TNCPU;
	//int TNIO;
	int CPU[MAX_CPU_BURST];
	int IO[MAX_CPU_BURST-1];
} all_process[MAX_PROCESS];

void myfunc() {
  cout<<"This func takes no parameters and returns nothing";
  return /*nothing*/;
}


int string_spliter(string line, int *p_attributes, string delimiter){
	size_t prev = 0, pos;
	int att_count = 0;
	while ((pos = line.find_first_of(delimiter, prev)) != string::npos){
		if (pos > prev){
			string temp_str = line.substr(prev, pos-prev);
			p_attributes[att_count] = atoi(temp_str.c_str());
			att_count++;
		}
		prev = pos + 1;
	}
	return att_count;
}

void string_parser(string line, process *out_process, int p_index){
	int total_att;
	int att[MAX_CPU_BURST*2+3];
	int cpu_c = 0;
	int io_c = 0;
	total_att = string_spliter(line, att, " ");
	out_process[p_index].PID = att[0];
	out_process[p_index].TARQ = att[1];
	out_process[p_index].PRIO = att[2];
	out_process[p_index].TNCPU = att[3];
	for (int i = 4; i < total_att; i++){
		if ((i%2) == 0){ 
			out_process[p_index].CPU[cpu_c] = att[i];
			cpu_c++;
		}
		else { 
			out_process[p_index].IO[io_c] = att[i];
			io_c++;
		}
	}
	//out_process[p_index].TNCPU = att[3];
}



int main(){ 
	ifstream myfile;
	string line;
	myfile.open("E:/example.txt");
	//myfile<<"Writing this to a file..... \n";
	int line_counter = 0;
	if (myfile.is_open()){
		while (myfile.good()){
			getline (myfile,line);
			string_parser (line, all_process, line_counter);
			line_counter ++;
		}
	}
	//myfunc();
	myfile.close();
	for (int i = 0; i< line_counter; i++){
		cout <<"Process: "<<all_process[i].PID<<" ";
		cout <<all_process[i].TARQ<<" ";
		cout <<all_process[i].PRIO<<" ";
		cout <<all_process[i].TNCPU<<endl;
		cout <<"CPU/IO Burst: ";
		for (int j=0; j<all_process[i].TNCPU; j++){
			cout <<all_process[i].CPU[j]<<" "<<all_process[i].IO[j]<<" ";
		}
		cout <<endl;
	}
	cout <<"Press enter to exit"<<endl;
	string temp;
	cin >> temp;
	cout<<"terminating..."<<endl;
	return 0;
}

