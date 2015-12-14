#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <time.h>
#include "randomOS.cpp"
#include "process_input.cpp"
using namespace std;


//int_2_string from randomOS imported with process_activity


int uniprogrammed(char* file_name, int verbose)
{
	vector< vector<int> > main_vector = read_file(file_name); //vector of vectors... not important

	//vector of structs.. tha main vector
	vector<process_struct> process_list = return_vector_of_processes(main_vector);

	int spotlight_num = -1;
	int number_of_terminated = 0;
	vector<process_struct> ready_queue;
	int process_uber = 0; //watches the arrival time of processes in process_list
	int cycle_number = 0;


	while (number_of_terminated < process_list.size())
	{
		if (verbose == 1)
		{
			//pre cycle
			string pre_cycle_string = "Before cycle		";
			pre_cycle_string += int_2_string(cycle_number);
			pre_cycle_string += ": ";
			for (int i = 0; i < process_list.size(); ++i)
			{
				pre_cycle_string += process_list[i].current_status;
				pre_cycle_string += "  ";
				if (process_list[i].current_status == "blocked")
					pre_cycle_string += int_2_string(process_list[i].time_left_in_current_IO_burst);
				else if(process_list[i].current_status == "running")
					pre_cycle_string += int_2_string(process_list[i].time_left_in_current_CPU_burst);
				else
					pre_cycle_string += "0";
				pre_cycle_string += "		";
			}
			
			cout<< pre_cycle_string << endl;
		}

		//cycle

		//first check for anyone arriving in this cycle
		while(process_list[process_uber].arrival_time == cycle_number && process_uber < process_list.size())
		{
			ready_queue.push_back(process_list[process_uber]);
			process_list[process_uber].current_status = "ready";
			process_uber++;
		}

		//then the main shit

		//this only happens in the first cycle
		if (spotlight_num == -1)
		{
			spotlight_num = ready_queue[0].process_number;
			ready_queue.erase(ready_queue.begin());
			//initialize_run
			int t = randomOS(process_list[spotlight_num].b, verbose);
			process_list[spotlight_num].current_status = "running";
			process_list[spotlight_num].current_CPU_burst = t;
			process_list[spotlight_num].time_left_in_current_CPU_burst = t;
			

		}

		//then every cycle after the first
		else
		{
			if (process_list[spotlight_num].current_status == "running")
			{
				if(process_list[spotlight_num].time_left_in_current_CPU_burst > 1)
				{	//still needs to run some more
					// running_function
					process_list[spotlight_num].total_cpu_time_left -= 1;
					process_list[spotlight_num].time_left_in_current_CPU_burst -= 1;
				}

				else //time left is 1 or 0
				{	
					if (process_list[spotlight_num].time_left_in_current_CPU_burst == 1)
					{	//run then block or terminate
						// running_function
						process_list[spotlight_num].total_cpu_time_left -= 1;
						process_list[spotlight_num].time_left_in_current_CPU_burst -= 1;
						if (process_list[spotlight_num].total_cpu_time_left <= 0)
						{	//completely done running -> terminated
							process_list[spotlight_num].current_status = "terminated";
							process_list[spotlight_num].finishing_time = cycle_number;
							number_of_terminated++;
							//cout << "something was just terminated" << endl;

							if (number_of_terminated == process_list.size())
								break;

							spotlight_num = ready_queue[0].process_number;
							ready_queue.erase(ready_queue.begin());
							//initialize_run
							int t = randomOS(process_list[spotlight_num].b, verbose);
							process_list[spotlight_num].current_status = "running";
							process_list[spotlight_num].current_CPU_burst = t;
							process_list[spotlight_num].time_left_in_current_CPU_burst = t;
						}
						else
						{	//done running... for now
							//initialize_block
							process_list[spotlight_num].current_status = "blocked";
							process_list[spotlight_num].current_IO_burst = process_list[spotlight_num].m * process_list[spotlight_num].current_CPU_burst;
							process_list[spotlight_num].time_left_in_current_IO_burst = process_list[spotlight_num].current_IO_burst;
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
			else if(process_list[spotlight_num].current_status == "blocked")
			{
				if (process_list[spotlight_num].time_left_in_current_IO_burst > 1)
				{
					//io_function
					process_list[spotlight_num].time_left_in_current_IO_burst -= 1;
					process_list[spotlight_num].IO_time += 1;
				}
				else
				{	//time left in current IO burst 1 or 0
					//io_function once more then run 
					process_list[spotlight_num].time_left_in_current_IO_burst -= 1;
					process_list[spotlight_num].IO_time += 1;
					//initialize_run
					int t = randomOS(process_list[spotlight_num].b, verbose);
					process_list[spotlight_num].current_status = "running";
					process_list[spotlight_num].current_CPU_burst = t;
					process_list[spotlight_num].time_left_in_current_CPU_burst = t;
				}
			}
		}

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
	cout<< "The scheduling algorithm used was Uniprogrammed" << "\n" << endl;


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
		uniprogrammed(argv[2], 1);
	}
	else
	{
		uniprogrammed(argv[1], 0);
	}

	return 0;
}