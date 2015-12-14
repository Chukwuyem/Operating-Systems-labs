#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
#include <fstream>
#include <algorithm>    // std::sort
using namespace std;

struct process_struct{
	int process_number; //its number in the input list, i.e. PO, P1, P2
	int arrival_time;
	int b; //divisor for randomOS
	int total_cpu_time;
	int total_cpu_time_left;
	int current_CPU_burst;
	int time_left_in_current_CPU_burst; //decreases by 1 during running
	int m; // multiplier for I/O burst
	int current_IO_burst;
	int time_left_in_current_IO_burst; //decreases by 1 during blocking
	string current_status;
	int IO_time; //increases by 1 during blocking
	int waiting_time; // cumulative... time in waiting state
	int finishing_time; //cumulative while the process is not terminated
};

process_struct create_process(int a, int b, int c, int m, int p){
	//creates a process
	// -1 is an init value used for values that will be changed... 
	//when the process begins runs and what not, -1 will never be used
	//0 is an init value used for cumulative values that will be added on...
	//when the process begins runs and what not
	process_struct new_process;
	new_process.process_number = p;
	new_process.arrival_time = a;
	new_process.b = b;
	new_process.total_cpu_time = c;
	new_process.total_cpu_time_left = c;
	new_process.current_CPU_burst = -1;
	new_process.time_left_in_current_CPU_burst = -1;
	new_process.m = m;
	new_process.current_IO_burst = -1;
	new_process.time_left_in_current_IO_burst = -1;
	new_process.current_status = "unstarted";
	new_process.IO_time = 0;
	new_process.waiting_time = 0;
	new_process.finishing_time = -1;
	return new_process;
}




vector<int> string_to_vector(string raw_process)
{
	vector<int> a_process_vector;
	int buf; //buffer int
	stringstream ss(raw_process);

	while (ss >> buf)
		a_process_vector.push_back(buf);

	return a_process_vector;
}

string conv_int_2_string(int argument){
	//int a = 10;
	stringstream ss;
	ss << argument;
	string str = ss.str();
	return str;
}

void v_printer(vector<int> v)
{

	cout<< "( ";
	for (std::vector<int>::iterator i = v.begin(); i != v.end(); i+=1)
		cout << *i << " ";
	cout << " )";
}

bool comp_process (const std::vector<int>& a, const std::vector<int>& b) 
{
	return (a[0] < b[0]);
}

vector< vector<int> > read_file(char* filename){
	//returns a vector of vectors
	//each vector in the main vector represenst [A B C M] for a process
	//the vectors are arranged by arrival time, hence the first vector 
	//is the earliest process to begin and so on

	vector< vector<int> > input_list_of_process;

	//opening file
	ifstream my_file;
	my_file.open(filename);
	if (my_file.is_open())
	{
		string line;
		if (getline(my_file, line))
		{
			//splitting the line
			size_t pos = 0;
			string token;
			string delimiter = " (";
			while ((pos = line.find(delimiter)) != std::string::npos) {
			    token = line.substr(0, pos);
			    if (token.length() > 1)
			    {
			    	char sec_delim = ')';
					size_t t_pos = token.find(sec_delim);
					token = token.substr(0, t_pos);
			    	vector<int> token_vector = string_to_vector(token);
			    	input_list_of_process.push_back(token_vector);

			    	//v_printer(token_vector);

			    }
			    //cout << token << endl;

			    line.erase(0, pos + delimiter.length());
			}
			char sec_delim = ')';
			size_t t_pos = line.find(sec_delim);
			line = line.substr(0, t_pos);
			//cout<< line << endl;
			vector<int> line_vector = string_to_vector(line);
			input_list_of_process.push_back(line_vector);

			//v_printer(line_vector);

		}
	}
	else
	{
		cout<< "Error: file not open" << endl;
	}
	my_file.close();

	cout << "The original input was: "<<  conv_int_2_string(input_list_of_process.size()) << " ";
	for (int i = 0; i < input_list_of_process.size(); ++i)
	{
		v_printer(input_list_of_process[i]);
	}
	cout << "\n";


	if (input_list_of_process.size() > 0)
		std::sort (input_list_of_process.begin(), input_list_of_process.end(), comp_process);

	cout << "The (sorted) input is: "<< conv_int_2_string(input_list_of_process.size()) << " ";
	for (int i = 0; i < input_list_of_process.size(); ++i)
	{
		v_printer(input_list_of_process[i]);
	}
	cout << "\n";

	return input_list_of_process;
}

vector<process_struct> return_vector_of_processes(vector< vector<int> > input_processes_list)
{
	vector<process_struct> vector_of_processes;

	for (int i = 0; i < input_processes_list.size(); ++i)
	{
		process_struct process = create_process(input_processes_list[i][0], input_processes_list[i][1], input_processes_list[i][2], input_processes_list[i][3], i);
		vector_of_processes.push_back(process);
	}

	return vector_of_processes;
}

// int main()
// {
// 	char file[] = "input_4";
// 	vector< vector<int> > main_vector = read_file(file);
// 	cout<< "main function" << endl;
// 	if (main_vector.size() > 0)
// 	{
// 		for (int i = 0; i < main_vector.size(); ++i)
// 		{
// 			v_printer(main_vector[i]);
// 		}
// 	}

// 	vector<process_struct> process_list = return_vector_of_processes(main_vector);

// 	//process_list.erase(process_list.begin());

// 	for (int i = 0; i < process_list.size(); ++i)
// 	{
// 		cout << process_list[i].arrival_time << endl;
// 	}


	
// 	return 0;
// }