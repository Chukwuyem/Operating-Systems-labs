#include <iostream>
#include <vector>
#include <string>
#include <time.h>
#include "randomOS.cpp"
#include "process_input.cpp"
using namespace std;


//int_2_string from randomOS imported with process_activity

bool sjf_comp (const process_struct& a, const process_struct& b) 
{
	return (a.total_cpu_time_left < b.total_cpu_time_left);
}


int sjf(char* filename, int verbose)
{
	vector< vector<int> > main_vector = read_file(filename); //vector of vectors... not important

	//vector of structs.. tha main vector
	vector<process_struct> process_list = return_vector_of_processes(main_vector);

	int rp_num = -1; //running process num
	int number_of_terminated = 0;
	vector<process_struct> ready_queue;
	vector<process_struct> block_queue;
	int process_uber = 0; //watches the arrival time of processes in process list
	int cycle_number = 0;


	while (number_of_terminated < process_list.size())
	{
		if (verbose == 1)
		{
			//pre cycle
			string pre_cycle_string = "Before cycle		";
			pre_cycle_string += int_2_string(cycle_number);
			pre_cycle_string += ": ";
			int w;
			for (int i = 0; i < process_list.size(); ++i)
			{
				pre_cycle_string += process_list[i].current_status;
				pre_cycle_string += "  ";
				if (process_list[i].current_status == "blocked")
				{
					w = process_list[i].time_left_in_current_IO_burst;
					pre_cycle_string += int_2_string(process_list[i].time_left_in_current_IO_burst);
				}
				else if(process_list[i].current_status == "running")
					pre_cycle_string += int_2_string(process_list[i].time_left_in_current_CPU_burst);
				else
					pre_cycle_string += "0";
				pre_cycle_string += "		";
			}
			
			cout<< pre_cycle_string << endl;
		}

		//cycle

		//we do the process uber last
		//because the earlier programs have to be added to ready queue
		//before just arrivers

		//this only happens in first cycle
		if (rp_num == -1)
		{
			if (process_list[0].process_number == cycle_number)
			{	//take the first process directly from the main vector to running 
				//since ready queue is definitely empty at this 
				rp_num = process_list[0].process_number;
				//initialize run
				int t = randomOS(process_list[rp_num].b, verbose);
				process_list[rp_num].current_status = "running";
				process_list[rp_num].current_CPU_burst = t;
				process_list[rp_num].time_left_in_current_CPU_burst = t;
				process_uber++; //because something was just added from process_list
				//hence, we don't want to cause conflict when the real uber go to process_list
			}
			//else no process starts at cycle number 0 or whatever cycle number is going on right now...
			//most likely, most processes have terminated and the last guy(s) are blocked
		}

		//then every cycle after the first
		else
		{
			if (process_list[rp_num].current_status == "running") //it will most definitely always be
			{
				if (process_list[rp_num].time_left_in_current_CPU_burst > 1)
				{	//still needs to run some more
					//running_function
					process_list[rp_num].total_cpu_time_left -= 1;
					process_list[rp_num].time_left_in_current_CPU_burst -= 1;
				}

				else //time left in burst is 1 or 0
				{
					if (process_list[rp_num].time_left_in_current_CPU_burst == 1)
					{	//run then block terminate
						//running_function
						process_list[rp_num].total_cpu_time_left -= 1;
						process_list[rp_num].time_left_in_current_CPU_burst -= 1;
						if (process_list[rp_num].total_cpu_time_left <= 0)
						{	//completely done running -> terminate
							process_list[rp_num].current_status = "terminated";
							process_list[rp_num].finishing_time = cycle_number;
							number_of_terminated++;

							if (number_of_terminated == process_list.size())
								break;

							if (ready_queue.size() > 0)
							{
								std::sort (ready_queue.begin(), ready_queue.end(), sjf_comp);
								rp_num = ready_queue[0].process_number;
								ready_queue.erase(ready_queue.begin());
								//initialize run
								int t = randomOS(process_list[rp_num].b, verbose);
								process_list[rp_num].current_status = "running";
								process_list[rp_num].current_CPU_burst = t;
								process_list[rp_num].time_left_in_current_CPU_burst = t;
							}
							else	//ready queue is empty
								rp_num = -1;
						}
						else
						{	//done running... for now
							//initialize_block
							process_list[rp_num].current_status = "blocked";
							process_list[rp_num].current_IO_burst = process_list[rp_num].m * process_list[rp_num].current_CPU_burst;
							process_list[rp_num].time_left_in_current_IO_burst = process_list[rp_num].current_IO_burst;
							block_queue.push_back(process_list[rp_num]);
							//since the running process just blocked, find something to run
							if (ready_queue.size() > 0)
							{
								std::sort (ready_queue.begin(), ready_queue.end(), sjf_comp);
								rp_num = ready_queue[0].process_number;
								ready_queue.erase(ready_queue.begin());
								//initialize run for the process in "spotlight"
								int t = randomOS(process_list[rp_num].b, verbose);
								process_list[rp_num].current_status = "running";
								process_list[rp_num].current_CPU_burst = t;
								process_list[rp_num].time_left_in_current_CPU_burst = t;
							}
							else	//ready queue is empty
								rp_num = -1;
						}
					}
					else
					{
						cout << "You shouldn't be here" << endl;
						//cout << "process_list[spotlight_num].time_left_in_current_CPU_burst is 0" << endl;
						break;
					}
				}
			}
			else
			{
				cout << "Something is wrong, why is running process not running?" << endl;
				break;
			}
		}

		//run IOs i.e. blocks
		if (block_queue.size() > 0)
		{
			for (int i = 0; i < block_queue.size(); ++i)
			{
				int p_num = block_queue[i].process_number;
				for (int x = 0; x < process_list.size(); ++x)
				{
					if (process_list[x].process_number == p_num)
					{
						if (process_list[x].time_left_in_current_IO_burst > 1)
						{	
							//io_function
							process_list[x].time_left_in_current_IO_burst -= 1;
							process_list[x].IO_time += 1;
						}
						else
						{	//time left in current IO burst 1 or 0
							//io_function once then ready or if possible run
							process_list[x].time_left_in_current_IO_burst -= 1;
							process_list[x].IO_time += 1;
							if (rp_num == -1)
							{	//can be run
								//initialize run
								rp_num = process_list[x].process_number;
								int t = randomOS(process_list[x].b, verbose);
								process_list[x].current_status = "running";
								process_list[x].current_CPU_burst = t;
								process_list[x].time_left_in_current_CPU_burst = t;
							}
							else
							{
								ready_queue.push_back(process_list[x]);
								process_list[x].current_status = "ready";
							}
							//either way it is no longer being blocked
							block_queue.erase(block_queue.begin()+i);
						}
					}
				}
			}
		}

		//now we can do process uber since they will be the last to be added to ready queue in each cycle

		while(process_list[process_uber].arrival_time == cycle_number && process_uber < process_list.size())
		{
			ready_queue.push_back(process_list[process_uber]);
			process_list[process_uber].current_status = "ready";
			process_uber++;
		}

		//now, the guys who are waiting
		if (ready_queue.size() > 0)
		{
			for (int i = 0; i < ready_queue.size(); ++i)
			{
				int p_num = ready_queue[i].process_number;
				for (int e = 0; e < process_list.size(); ++e)
				{
					if (process_list[e].process_number == p_num)
						process_list[e].waiting_time++;
				}
			}
		}

		cycle_number++;
	}

	cout<< "The scheduling algorithm used was Shortest Job First" << "\n" << endl;

	if (number_of_terminated < process_list.size())
		cout << "something's wrong" << endl;
	else
		cout << "yup, you wrote it" << endl;

	
	double totalIOtime = 0;
	double totalCPUtimeForAll = 0;
	int waiting_time = 0;
	int turnaround_time = 0;

	for (int i = 0; i < process_list.size(); ++i)
	{
		/* code */
		cout << "Process "<< process_list[i].process_number << endl;
		cout << "(A, B, C, M) = (" << process_list[i].arrival_time <<", "<<process_list[i].b<<", "<<process_list[i].total_cpu_time<<", "<<process_list[i].m<<")" << endl;
		cout << "Finishing time: "<< process_list[i].finishing_time <<  endl;
		cout << "Turnaround time: " << process_list[i].finishing_time - process_list[i].arrival_time << endl;
		cout<< "I/O time: " << process_list[i].IO_time << endl;
		cout << "Waiting time: " << process_list[i].waiting_time << endl;
		cout << "\n" ;

		totalIOtime += process_list[i].IO_time;
		totalCPUtimeForAll += process_list[i].total_cpu_time;
		waiting_time += process_list[i].waiting_time;
		turnaround_time += (process_list[i].finishing_time - process_list[i].arrival_time);
	}

	double cpu_util =  totalCPUtimeForAll / cycle_number;
	double io_util = totalIOtime / cycle_number;
	double ave_waiting_time = waiting_time / process_list.size();
	double ave_turnaroung_time = turnaround_time / process_list.size();

	cout << "Finishing time: " << cycle_number << endl;
	cout << "CPU Utilization: " << cpu_util << endl;
	cout << "I/O Utilization: " << io_util << endl;
	cout.precision(6);
	cout << "Average Turnaround Time: " << ave_turnaroung_time << endl;
	cout << "Average Waiting Time: " << ave_waiting_time << endl;


	return 0;
}

int main(int argc, char* argv[])
{
	/* initialize random seed: */
	//used for the randomOS that will be run later on
	srand (time(NULL));

	char* verb = argv[1];
	string verb_test = "--verbose";

	if (verb_test.compare(verb) == 0)
	{
		sjf(argv[2], 1);
	}
	else
	{
		sjf(argv[1], 0);
	}

	return 0;
}