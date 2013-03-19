#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "processt.hpp"
#include "gantt_data.hpp"
#include <vector>

using namespace std;

//#ifndef MAX_CPU_BURST 100
	#define MAX_CPU_BURST 100
//#endif
//#define MAX_IO_BURST 100
#define MAX_PROCESS 100

processt all_process[MAX_PROCESS];

typedef void (*schedule_t) (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef);
int string_spliter(string, int*, string);
processt string_parser(string);
void similator (
	vector<processt> process_list, 
	schedule_t schedule_function, 
	vector<processt> *finish_list, 
	vector<gantt_data> *gantt_data_list, 
	int age_scale,
	bool round_robin,
	int quantum_time,
	float weight_coef,
	bool impatient_prio
	);

int set_parameters (
	int algorithm_index,
	bool *round_robin,
	bool *impatient_prio);

void result_display (vector<processt> *process_list, vector<gantt_data> *gd_list);
/*
void test_function (vector<processt> *ready_list){
	(*ready_list).front().PID = 300;
	cout<<"test1 "<<(*ready_list).front().PID<<endl;
}*/


//Algorithm Template
//=============================================================

void FCFS (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef);
void priority_npr (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef);
void SJF (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef);
void SPB (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef);
/*
bool schedule2 (process_t *process_list, int process_c){
	process_t process_local[MAX_PROCESS];
	memcpy (process_local, process_list, MAX_PROCESS);
	process_local[0].PID = 999;
	cout <<"test2"<<process_list[0].PID<<endl;
	cout <<"test1"<<process_local[0].PID<<endl;
	
	return true;
}*/
//=============================================================


int main(){ 
	ifstream myfile;
	string line;
	string path;
	string end_options;
	do{
	do{ 
		cout << "Please enter the absolute path of the file: ";
		cin >> path;
		//path = "e:/example.txt";
		myfile.open(path.c_str());
	}while (!myfile.is_open());


	int line_counter = 0;
	vector<processt> all_process;
	
	while (myfile.good()){
		getline (myfile,line);
		all_process.push_back(string_parser(line));
	};

	myfile.close();//finish reading and parsing file
	do{
	cout<<"Please select schedule algorithm:"<<endl;
	cout<<"0 First Come First Serve"<<endl<<"1 Priority NPR"<<endl<<"2 Round Robin"<<endl<<"3 Shortest Job First"<<endl<<"4 Shortest Previous Burst"<<endl<<"5 Impatient Priority"<<endl<<"6 Polite Priority"<<endl;
	//initialize parameters for similation
	//0. FCFS
	//4. SPB
	//5. Impatient Prio
	//6. Prio+RR
	int algorithm_index;
	string algorithm_input;
	vector<processt> finish_list;
	vector<gantt_data> gantt_data_list;
	int age_scale = 20;
	bool round_robin;
	int quantum_time = 10;
	bool impatient_prio = true;
	float weight_coef =0.5 ;

	bool algorithm_check;
	do{
	//cout<<"sup"<<endl;
	cin>>algorithm_input;
	algorithm_check = true;
	if(algorithm_input == "0"){
		algorithm_index = 0;
	}
	else if(algorithm_input == "1"){
		algorithm_index = 1;
		cout<<"Please enter the age scale: ";
		cin>>age_scale;
	}
	else if(algorithm_input == "2"){
		algorithm_index = 2;
		cout<<"Please enter the quantum time: ";
		cin>>quantum_time;
	}
	else if(algorithm_input == "3"){
		algorithm_index = 3;
		cout<<"Please enter the age scale: ";
		cin>>age_scale;
	}
	else if(algorithm_input == "4"){
		algorithm_index = 4;
		cout<<"Please enter the age scale: ";
		cin>>age_scale;
		cout<<"Please enter the weight coefficient in decimal: ";
		cin>>weight_coef;
	}
	else if(algorithm_input == "5"){
		algorithm_index = 5;
		cout<<"Please enter the age scale: ";
		cin>>age_scale;
	}
	else if(algorithm_input == "6"){
		algorithm_index = 6;
		cout<<"Please enter the age scale: ";
		cin>>age_scale;
		cout<<"Please enter the quantum time: ";
		cin>>quantum_time;
	}
	else{
		cout<<"Please select a schedule algorithm"<<endl;
		algorithm_check = false;
	}
	}while(!algorithm_check);

	
	schedule_t schedule_functinos[] =
	{
		FCFS,
		priority_npr,
		SJF,
		SPB
	};

	int schedule_func_index = set_parameters(algorithm_index, &round_robin, &impatient_prio);
	
	similator(
		all_process, schedule_functinos[schedule_func_index], 
		&finish_list, 
		&gantt_data_list, 
		age_scale, 
		round_robin, 
		quantum_time,
		weight_coef,
		impatient_prio);
	
	result_display(&finish_list, &gantt_data_list);


	cout<<"All processes are finished executing."<<endl;
	cout<<"Please choose from the following options: "<<endl;

	
	cout<<"0: use the existing process data to run another schedule algorithm"<<endl;
	cout<<"1: start over by importing another file"<<endl;
	cout<<"others: quit"<<endl;
	cin>>end_options;
	}while(end_options == "0");
	}while(end_options == "1");
	//cin.ignore(); 
    //cin.get();
	cout<<"terminating..."<<endl;
	return 0;
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

processt string_parser(string line){//, process_t *out_process, int p_index){
	int total_att;
	int att[MAX_CPU_BURST*2+3];
	int cpu_c = 0;
	int io_c = 0;
	processt out_process;
	total_att = string_spliter(line, att, " ");
	out_process.PID = att[0];
	out_process.TARQ = att[1];
	out_process.PRIO = att[2];
	out_process.TNCPU = att[3];
	//initialize other variables in the process
	out_process.current_burst = 0;
	out_process.execution_time = 0;
	out_process.turnaround_time = 0;
	out_process.wait_time = 0;
	out_process.age = 0;
	out_process.average_burst = 0;
	out_process.last_burst = 0;

	for (int i = 4; i < total_att; i++){
		if ((i%2) == 0){ 
			out_process.CPU[cpu_c] = att[i];
			cpu_c++;
		}
		else { 
			out_process.IO[io_c] = att[i];
			io_c++;
		}
	}
	return out_process;
	//out_process[p_index].TNCPU = att[3];
}

void similator (
	vector<processt> wait_list, 
	schedule_t schedule_function, 
	vector<processt> *finish_list, 
	vector<gantt_data> *gantt_data_list, 
	int age_scale,
	bool round_robin,
	int quantum_time, 
	float weight_coef,
	bool impatient_prio
	){
	vector<processt> cpu;
	vector<processt> io_list;
	//vector<processt> finish_list;
	vector<processt> ready_list;
	int quantum_time_c = 0;
	int timer = 0;

	processt cpu_io_buffer;//buffer between cpu to io;
	vector<processt> io_ready_buffer;
	processt cpu_ready_buffer;
	bool cpu_io_ready = false;//indicates whether the buffer is ready
	bool io_ready_ready = false;
	bool cpu_ready_ready = false;

	while ((wait_list.size() != 0)||(cpu.size() != 0)||(io_list.size() != 0)||(ready_list.size() != 0)){ 
		
		//before the tick
		if (wait_list.size() != 0){
				for (int i = wait_list.size()-1; i>= 0; i--){
					if (wait_list[i].TARQ == timer){

						//wait_list[i].age = 0; //reset age before entering the ready list
						//-----------------------algorithm--------------------------------
						schedule_function(&ready_list,wait_list[i],age_scale,weight_coef);
						wait_list.erase(wait_list.begin()+i);
					}
				}
			}

		//if cpu is idle
		if (cpu.size() == 0){
			if(ready_list.size()!=0){
				cpu.push_back(ready_list.front());
				ready_list.erase(ready_list.begin());

				//for SPB, record the last burst before counting down the current cpu burst.
				cpu.front().last_burst = cpu.front().CPU[cpu.front().current_burst];

				//record gantt data
				gantt_data new_data;
				new_data.PID = cpu.front().PID;
				new_data.time = timer;
				if(
					((*gantt_data_list).size()!=0) &&
					((*gantt_data_list).back().PID == -1) &&
					((*gantt_data_list).back().time == timer)
					){
					(*gantt_data_list).back() = new_data;
				}
				else{
					(*gantt_data_list).push_back(new_data);
				}
				
				
			}
		}
		
		if((impatient_prio)&&(cpu.size()!=0)){
			if(ready_list.size()!=0){
				if ((cpu.front().PRIO - (cpu.front().age)/age_scale) > (ready_list.front().PRIO - (ready_list.front().age)/age_scale)){
					cpu.front().age = 0;
					schedule_function(&ready_list, cpu.front(), age_scale, weight_coef);
					cpu.erase(cpu.begin());
					quantum_time_c = 0;

					cpu.push_back(ready_list.front());
					ready_list.erase(ready_list.begin());

					gantt_data new_data;
					new_data.PID = cpu.front().PID;
					new_data.time = timer;
					(*gantt_data_list).push_back(new_data);
				}
			}
		}

		
		//during the tick
		timer++;

		//wait time count ++ for all processes in the ready list
		if (ready_list.size() != 0){
			for (int i = ready_list.size()-1; i>=0; i--){
				ready_list[i].wait_time++;
				ready_list[i].age++;
			} 
		}  

		//decrease the io count down for processes in the io_list
			if (io_list.size() != 0){
				for (int i = io_list.size()-1; i >=0; i--){
					if (io_list[i].IO[io_list[i].current_burst] > 0){
						io_list[i].IO[io_list[i].current_burst]--;
					}
					if (io_list[i].IO[io_list[i].current_burst] == 0){
						io_list[i].current_burst++;//current_burst is increased when an io burst is finished

						io_list[i].age = 0;//reset age before entering the ready lisst

						io_ready_buffer.push_back(io_list[i]);
						io_ready_ready = true;
						//-----------------------algorithm--------------------------------
						//schedule_function(&ready_list, io_list[i], age_scale, weight_coef);

						io_list.erase(io_list.begin()+i);
					}
				}
			}
			
			//decrease the cpu count down for processes in the cpu list
			if (cpu.size() != 0){
				if (cpu.front().CPU[cpu.front().current_burst] > 0){
					cpu.front().CPU[cpu.front().current_burst]--;
					cpu.front().execution_time++;//excution time count ++ 

					quantum_time_c++;
				}
				if (cpu.front().CPU[cpu.front().current_burst] == 0){
					//if all bursts are finished 
					if (cpu.front().current_burst>=(cpu.front().TNCPU-1)){
						cpu.front().turnaround_time = timer;//save the current time as turnaround time
						(*finish_list).push_back(cpu.front());
					}
					else{
						//io_list.push_back(cpu.front());
						cpu_io_ready = true;
						cpu_io_buffer = cpu.front();
					}

					//record gantt data
					gantt_data new_data;
					new_data.PID = -1;
					new_data.time = timer;
					(*gantt_data_list).push_back(new_data);


					cpu.erase(cpu.begin());

					quantum_time_c = 0;
				}

				//if quantum time count is up, given the round_robin algorithm is being used
				if((quantum_time_c>=quantum_time)&&(round_robin)){
					//cpu.front().age = 0;//reset age before entering the ready lisst

					cpu_ready_buffer = cpu.front();
					cpu_ready_ready = true;
					//going back to ready_list through the schedule function
					//-----------------------algorithm--------------------------------
					//schedule_function(&ready_list, cpu.front(), age_scale, weight_coef);
					
					//record gantt data
					gantt_data new_data;
					new_data.PID = -1;
					new_data.time = timer;
					(*gantt_data_list).push_back(new_data);

					cpu.erase(cpu.begin());

					quantum_time_c = 0;
				}
			}

			//after the tick
			if (io_ready_ready){
				for (int i = io_ready_buffer.size()-1; i >= 0; i--){
					schedule_function(&ready_list, io_ready_buffer[i], age_scale, weight_coef);
				}
				io_ready_buffer.clear();
				io_ready_ready = false;
			}

			if (cpu_ready_ready&&round_robin){
				schedule_function(&ready_list,cpu_ready_buffer, age_scale, weight_coef);
				cpu_ready_ready = false;
			}
			
			if (cpu_io_ready){
				io_list.push_back(cpu_io_buffer);
				cpu_io_ready = false;
			}

		
	}
	//ready_list.insert(ready_list.begin(), finish_list.begin(), finish_list.end());
	return;
}

void result_display (vector<processt> *process_list, vector<gantt_data> *gd_list){
	for(int i=0; i != (*process_list).size(); i++){
		cout <<"Process "<<(*process_list)[i].PID<<", ";
		cout <<"execution Time: "<<(*process_list)[i].execution_time<<", ";
		cout <<"wait time: "<<(*process_list)[i].wait_time<<", ";
		cout <<"turnaround time: "<<(*process_list)[i].turnaround_time<<endl;
		/*
		cout <<"CPU/IO Burst: ";
		for (int j=0; j<all_process[i].TNCPU; j++){
			cout <<all_process[i].CPU[j]<<" ";
			if(!(all_process[i].TNCPU == j+1))
				cout <<all_process[i].IO[j]<<" ";
		}*/
	}
	cout <<"draft gantt chat: "<<endl;
	for (int unsigned i=0; i<(*gd_list).size(); i++){
		cout <<"CPU Time: "<< (*gd_list)[i].time<<", PID: "<<(*gd_list)[i].PID<<endl;
	}
	
}

int set_parameters (
	int algorithm_index,
	bool *round_robin,
	bool *impatient_prio){
		*round_robin = false;
		*impatient_prio = false;
		if (algorithm_index == 0){//FCFS
			//*age_scale = 0;
			//*round_robin = false;
			return 0;
		}
		else if(algorithm_index == 1){//npr prio
			return 1;
		}
		else if(algorithm_index == 2){//round robin
			*round_robin = true;
			return 0;
		}
		else if(algorithm_index == 3){//sjf
			return 2;
		}
		else if(algorithm_index == 4){//SPB 
			return 3;
		}
		else if(algorithm_index == 5){//impatient prio
			*impatient_prio = true;
			return 1;
		}
		else if(algorithm_index == 6){//polite prio
			*round_robin = true;
			return 1;
		}
		else{
			return 0;
		}
}
		

void FCFS (vector<processt> *process_list, processt new_process, int arg, float weight_coef){
	(*process_list).push_back(new_process);
}

void priority_npr (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef){
	int unsigned count = 0;
	//bool inserted = false;
	//if ((*process_list).size() > 0){
	while(count< (*process_list).size() ){
			//compare priority with age being considered
		if (new_process.PRIO < ((*process_list)[count].PRIO - ((*process_list)[count].age)/age_scale)){
			(*process_list).insert((*process_list).begin()+count, new_process);
			return;
		}
		count++;
	}
	(*process_list).push_back(new_process);
	return;
}

void SJF (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef){ 
	int unsigned count = 0;
	while(count < (*process_list).size() ){
			if (new_process.CPU[new_process.current_burst] < 
				( (*process_list)[count].CPU[(*process_list)[count].current_burst] - ((*process_list)[count].age)/age_scale ) ){
				(*process_list).insert((*process_list).begin()+count, new_process);
				return;
			}
			count++;
	}
	(*process_list).push_back(new_process);
	return;
}

void SPB (vector<processt> *process_list, processt new_process, int age_scale, float weight_coef){ 
	int unsigned count = 0;
	float next_burst;
	float next_burst2;
	if (new_process.current_burst != 0){
		new_process.average_burst = (new_process.average_burst*(new_process.current_burst-1)+new_process.last_burst)/new_process.current_burst;
	}
	next_burst = weight_coef*new_process.last_burst + (1-weight_coef)*new_process.average_burst;
	while(count < (*process_list).size() ){
		next_burst2 = weight_coef*(*process_list)[count].last_burst + (1-weight_coef)*(*process_list)[count].average_burst;
		if (next_burst < (next_burst2 - ((*process_list)[count].age)/age_scale)){
			(*process_list).insert((*process_list).begin()+count, new_process);
				return;
			}
			count++;
		}
		(*process_list).push_back(new_process);
	return;
}
