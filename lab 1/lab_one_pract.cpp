/**
 * Author:    Chukwuyem Onyibe
 * NetID: cjo258
 *
 * Created for Operating Systems Lab 1
 * 
 **/


#include <iostream>
#include <string>
#include <vector>
#include <sstream> 
#include <fstream>
using namespace std;

struct sym_val{
	string symbol;
	int value;
	string errors;
	bool used;
};


//vector to store symbols and their values rep symbol table
vector<sym_val> symbol_table;
//function to find value of a symbol
int find_symbol_val(string symb)
{
	for (int i= 0; i < symbol_table.size(); i++)
	{
		if (symbol_table[i].symbol == symb)
			symbol_table[i].used = true;
			return symbol_table[i].value;
	}
	return -2; //the symbol is not defined error
}
//iterator for symbol_table;
int symbol_table_iterator = 0;
//function to check a symbol already exists in the symbol table vector
int already_exists(string new_symbol)
{
	int not_found = -1;
	if(!symbol_table.empty()) //if the table is not empty
	{
		for (int count = 0; count <  symbol_table.size(); count++)
		{
			if (symbol_table[count].symbol == new_symbol) //it already exist in the vector
				return count;
		}
	}
	else //the table is empty
	{
		return not_found;
	}
	return not_found;
}
//other important information to be extracted in first pass
int num_of_modules = 0;
//vector to store base addresses of each module
vector<int> module_addresses;
//vector to store size of modules
vector<int> module_sizes;

//this is done so, when use the vector, we know that module 1 has module address of module_address[0]
//module has add of module_addess[1] and so on
//current aboslute address: this number will represent the absolute address of the current module
int current_absolute_add = 0;

//some necessary functions
int string_2_int(string argument){
	int return_val;
	stringstream ss(argument);
	ss >> return_val;
	return return_val;
}

string int_2_string(int argument){
	//int a = 10;
	stringstream ss;
	ss << argument;
	string str = ss.str();
	return str;
}

string footnote_errors;

/*
the first pass should
1. get base address for each module
this is in every third line starting with line 3
3 6 9 12, i.e. x%3 == 0
2. create symbol table
this is in every third line starting with line 1
1 4 7 10, i.e. x%3 == 1
*/
int first_pass(char* file_name)
{
	//opening file
	ifstream my_file;
	my_file.open(file_name);
	string each_line;
	int line_num = 1;
	if (my_file.is_open())
	{
		module_addresses.push_back(0); //base address of module 1
		while (getline(my_file, each_line))
		{
			if (!each_line.empty())
			{
				if (line_num == 1 || line_num % 3 == 1) // definition lines
				{
					num_of_modules++;
					//splitting the line and putting it in a vector
					string buf; //buffer string
					stringstream ss(each_line);

					vector<string> tokens; //vector containing strings of symbols and their values
					while (ss >> buf)
						tokens.push_back(buf);
					
					//creating sym_val and adding to symbol_table
					for (std::vector<string>::iterator i = tokens.begin() + 1; i != tokens.end(); i+=2)
					//starts a tokens.begin() + 1 because the first value in the number of definitions
					{
						int existence_val = already_exists(*i);
						sym_val symbol_value; //creating sym_val instance
						symbol_value.symbol = *i; // this is the symbol
						symbol_value.used = false;
						string val = *(i + 1);
						stringstream ss(val);
						ss >> symbol_value.value; // this is the value
						symbol_value.value += current_absolute_add;

						if (existence_val > -1)  //it exists in the table, i.e. multiply defined
						{
							cout<< "it is "<< endl;
							//delete the old symbol_value from symbol_table
							symbol_table.erase(symbol_table.begin()+existence_val);
							//add error message to new symbol_value
							symbol_value.errors += "Error: This variable is multiply defined; last value used. ";
						}
						else
							; //doesn't exist in the table, i.e. it's a new symbol... do nothing
						symbol_table.push_back(symbol_value);
					}
				}
				else if (line_num % 3 == 0)
				{
					//no need to do num_of_modules++ because that has been taken care of in definition line
					//split the line to get base address
					string buf; //buffer string
					stringstream ss(each_line);
					ss >> buf; //buf is the base address of module (line_num / 3) + 1
					int module_base_add;
					stringstream ss2(buf);
					ss2 >> module_base_add;
					current_absolute_add += module_base_add;
					//check address defined for symbols just added to symbol_table if they exceed size of module

					// cout<< "running inspection" << endl;
					// cout<< "current_absolute_add: " << current_absolute_add << endl;
					for (int count = 0; count <  symbol_table.size(); count++)
					{
						if (symbol_table[count].value < current_absolute_add)
							; // do nothing... it's fine
						else //address in definition exceeds size of module
						{
							symbol_table[count].value = (current_absolute_add - 1);
							symbol_table[count].errors += "Error: Address in definition exceeds size of module. Last word of module assigned. ";
						}
					}
					module_sizes.push_back(module_base_add);
					module_addresses.push_back(current_absolute_add);
				}
				line_num += 1;
			}
		}
	}
	else
	{
		cout<< "Error opening file: "<< file_name << endl;
	}
	my_file.close();

	return 0;
}

/*
Pass two uses the base addresses and the symbol table computed in pass one to gen-
erate the actual output by relocating relative addresses and resolving external references.

the use list is on lines 2, 5, 8.... i.e. x % 3 == 2
*/

//vector that holds instructions for each module
vector<string> module_instructions;
string errors_foreach_module;

int second_pass(char* file_name)
{
	cout << "Memory map" << endl;
	int print_counter = 0;
	//opening file
	ifstream my_file;
	my_file.open(file_name);
	string each_line;
	int line_num = 1;
	if (my_file.is_open())
	{
		//vector containing string of symbols and their use addresses for this module
		//defined here because it is loaded on use line and processed on instruction line
		vector<string> use_tokens;
		while (getline(my_file, each_line))
		{
			 
			if (!each_line.empty())
			{
				//cout << "we're in line "<< line_num << endl;
				if (line_num == 2 || line_num % 3 == 2) // use line
				{
					//splitting the line and putting it in a vector
					string buf; //buffer string
					stringstream ss(each_line);
					while (ss >> buf)
						use_tokens.push_back(buf); 

					//test print was here ***for debugging purposes
				}
				else if(line_num % 3 == 0) //line that contains instruction word addresses
				{
					//split the line to get word addresses
					string buf;
					stringstream ss(each_line);
					ss >> buf;
					while (ss >> buf)
						module_instructions.push_back(buf);
					

					// ****4, i.e. x % 10 = 4
					if (use_tokens.size() > 1) //if there is any symbol usage in this model/ i.e. any external address
					{
						//resolving all the symbol usage, i.e. all the external addresses
						vector<string>::iterator iter = use_tokens.begin()+1;
						string current_symbol = *iter;
						iter++;
						while(iter != use_tokens.end())
						{
							while(*iter != "-1")
							{
								//resolve the word that is at position *iter
								int position_of_instruction = string_2_int(*iter);
								int symbol_val = find_symbol_val(current_symbol);
								if (position_of_instruction < module_instructions.size())
								{
									int current_word = string_2_int(module_instructions[position_of_instruction]);
									if (current_word/1000 > 9) //to check if another symbol has been used in this definition
									{
										if ( symbol_val >= 0) //symbol is defined
										{
											current_word = symbol_val + ((current_word/10000) * 1000);
											module_instructions[position_of_instruction] = int_2_string(current_word);
										}
										else //symbol is not defined
										{
											current_word = 111 + ((current_word/10000) * 1000);
											string err_str = " Error: " + current_symbol + " not defined. 111 used.";
											module_instructions[position_of_instruction] = (int_2_string(current_word).append(err_str) );
										}
									}
									else //it has already been used definition
									{
										if(footnote_errors.find("Error: mutliple symbols used in instruction "+*iter+" in module "+int_2_string((line_num/3)-1)+". All but last use ignored.\n") != string::npos)
											; //found!!!, i.e. already exists, therefore don't add the same thing
										else
											footnote_errors.append("Error: mutliple symbols used in instruction "+*iter+" in module "+int_2_string((line_num/3)-1)+". All but last use ignored.\n");
										if ( symbol_val >= 0) //symbol is defined
										{
											current_word = symbol_val + ((current_word/1000) * 1000);
											module_instructions[position_of_instruction] = int_2_string(current_word);
										}
										else //symbol is not defined
										{
											current_word = 111 + ((current_word/1000) * 1000);
											string err_str = " Error: " + current_symbol + " not defined. 111 used.";
											module_instructions[position_of_instruction] = (int_2_string(current_word).append(err_str) );
										}
									}
								}
								else
									footnote_errors.append("Error: Use of " + current_symbol + " in module "+ int_2_string(line_num/3) + "exceeds module size. Use ignored.\n");
								iter++;
							}
							
							if (iter == (use_tokens.end()- 1))
							{//end of use list
								break;
							}
							else
							{
								iter += 2;
								current_symbol = *(iter - 1);
							}	
						}
					}

					//for the other instruction words (immediate, absolute and relative)
					for (int i = 0; i < module_instructions.size(); i++)
					{
						if(module_instructions[i].size() == 5) //because the already resolved external addresses have size of 4
						{
							int instruction_word = string_2_int(module_instructions[i]);
							int outputed_word;
							if( instruction_word % 10 == 1)
							{
								module_instructions[i] = int_2_string (instruction_word / 10);
							}
							else if(instruction_word % 10 == 2)
							{
								if((instruction_word / 10) % 1000 > 299)
								{
									module_instructions[i] = int_2_string(299 + (1000 * (instruction_word / 10000)));
									module_instructions[i].append(" Error: Absolute address exceeds machine size; largest address used.");
								}
								else
									module_instructions[i] = int_2_string (instruction_word / 10);
							}
							else if(instruction_word % 10 == 3)
							{
								if((instruction_word / 10) + module_addresses[(line_num/3)-1] > module_addresses[(line_num/3)])
								{
									module_instructions[i] = int_2_string((1000 *(instruction_word/10000)) + (module_addresses[(line_num/3)]-1)).append(" Error: Relative address exceeds module size; largest module address used.\n");
									//replace its address with largest module address
								}
								else
									module_instructions[i] = int_2_string((instruction_word / 10) + module_addresses[(line_num/3)-1]);
									//instruction word + model base address
							}
						}
					}

					//test print was here ***for debugging purposes

					for (vector<string>::iterator i = module_instructions.begin(); i != module_instructions.end(); ++i)
					{
						cout << print_counter << ":\t" << *i << endl;
						print_counter++;
					}



					//after using instructions, clear the vector for the next module
					module_instructions.clear();
					//after using use_tokens, clear the vector for the next module
					use_tokens.clear();
				}
				line_num += 1;
			}
		}
	}
	else
	{
		cout<< "Error opening file: "<< file_name << endl;
	}
	my_file.close();

	return 0;
}


//MUST BE RUN WITH THE INPUT FILE AS AN ARGUMENT
int main(int argc, char* argv[]){
	if (argv[1])
	{
		first_pass(argv[1]);

		cout << "\n";
		cout<< "Symbol Table" << endl;
		for (int count = 0; count <  symbol_table.size(); count++)
		{
			if (!symbol_table[count].errors.empty())
			{
				cout<< symbol_table[count].symbol << "=" << symbol_table[count].value << " " << symbol_table[count].errors << endl;
			}
			else
			{
				cout<< symbol_table[count].symbol << "=" << symbol_table[count].value << endl;	
			}
		}
		cout << "\n";

		//test print was here ***for debugging purposes [module addresses & module sizes]

		second_pass(argv[1]);

		for (int w=0; w < symbol_table.size(); w++)
		{
			if(symbol_table[w].used == false)
			{
				footnote_errors.append("Warning: "+symbol_table[w].symbol+" was defined but never used.\n");
			}
		}
		cout<< footnote_errors << endl;
	}
	else	//keyboard input
		cout<< "You did not enter an file name." << endl;
	
	return 0;
}