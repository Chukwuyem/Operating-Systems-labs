#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;



string int_2_string(int argument){
	//int a = 10;
	stringstream ss;
	ss << argument;
	string str = ss.str();
	return str;
}

int randomOS(int U, int verbose)
{
	char file[] = "random_numbers.txt";
	char* file_name = file;
	int rand_line_num; //randomly generated line number
	int X; //the number to be sourced from the random number file
	int t; //final number to be returned

	/*random seed initialized later in the main program*/
  	

  	/* generate number between 1 and 100000: */
  	rand_line_num = rand() % 100000 + 1;
  	//cout<< "rand_line_num is "<< rand_line_num << endl;

	//opening file
	ifstream number_file;
	number_file.open(file_name);
	string each_line;
	int line_num = 1;

	if(number_file.is_open())
	{
		getline(number_file, each_line);
		while(rand_line_num != line_num)
		{
			line_num += 1;
			getline(number_file, each_line);
		}
		//at this point, each_line is the number we want in string form
		stringstream ss(each_line);
		ss >> X;
		//cout << "X is "<< X << endl;

		t = 1 + (X % U); //U is B
		//cout << "t is "<< t << endl;


		if (verbose == 1)
		{
			string print_str ="Find burst when choosing ready process to run: ";
			print_str += "X is ";
			print_str += int_2_string(X);
			print_str +=", t is ";
			print_str += int_2_string(t);

			cout<< print_str << endl;
		}

		

		return t;
	}
	else
		cout<< "Error opening file: "<< file_name << endl;

	number_file.close();

	return -1;
}

// int main()
// {
// 	srand (time(NULL));

// 	int r = 0;
// 	for (int i = 0; i < 8; ++i)
// 	{
// 		r = randomOS(20);
// 		cout << r << endl;
// 	}

// 	return 0;
// }