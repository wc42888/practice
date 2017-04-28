#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

using namespace std;

void check_indices(string &str);

vector<string> split_string(string &str);

bool is_digits(string &str);

bool check_format(string &str);


int main(int argc, char** argv){

	string input;

	//ask user to type in string
	cout<<"please enter your string: "<<endl;	
	getline(cin,input);

	//check if the input contains only 'x' and 'y'
	while(!check_format(input)){
		cout<<"input string does not match the format, please re-enter"<<endl;
		getline(cin,input);
	}

	check_indices(input);

	return 0;
}

void check_indices(string &str){

	cout<<"please enter the indices:"<<endl;
	
	string input;
	vector<string> answers;
	bool input_error = false;

	//keep getting input from the user until the EOF control-D is entered
	while(getline(cin,input)){
		
		//split the input strings into small pieces for error checking
		vector<string> input_strings=split_string(input);

		//check if there are more than two number entered
		if(input_strings.size()!=2){
			//cout<<"invalid number of inputs"<<endl;
			answers.push_back("invalid number of inputs");
			continue;	
		}

		//check if all the strings contain only numbers
		for (int i = 0; i<input_strings.size(); ++i){
			if(!is_digits(input_strings[i])){
				input_error = true;
				break;
			}
		}

		if (input_error){
			//cout<<"invalid input type"<<endl;
			answers.push_back("invalid input type");
			input_error=false;
			continue;
		}

		//convert string into integers
		int i = stoi(input_strings[0]);
		int j = stoi(input_strings[1]);

		// find the maximum and minimum between two integer
		int start = min(i,j);
		int end = max(i,j);

		//check if the indices is out of the range of the orginal string
		if (start<0 || end> str.size()){
			//cout<<"indices out of the range"<<endl;
			answers.push_back("indices out of the range");
			continue;
		}	

		//check the elements between two indices
		for (string::iterator it = str.begin()+start, first= str.begin()+end; it!=str.begin()+end+1; ++it){
			if(*(it)!=*first){
				//cout<<"false"<<endl;
				answers.push_back("false");
				break;			
			}
			if(*(it)==*first && it==str.begin()+end){
				//cout<<"true"<<endl;
				answers.push_back("true");
			}
		}
		
	}

	//add a new line to separate the input and the results
	cout<<endl;

	//print out the results all at once
	for (int i =0; i<answers.size(); i++){

		cout<<answers[i]<<endl;
	}

}

//split the string into a vector of strings at the each "space" character
vector<string> split_string(string &str){
	vector<string> strings;
	size_t start = 0;
	size_t end = 0;

	//split the strings at "space" character
	while((end=str.find(" ",start))!=string::npos){
    		strings.push_back(str.substr(start, end - start));
    		start = end + 1;
  	}

	//push the last string into the vector
	strings.push_back(str.substr(start));
	return strings;
}

//check if a string contains only numbers
bool is_digits(string &str){
	return str.find_first_not_of("0123456789") == std::string::npos;
}

//check if a string contains only 'x' and 'y'
bool check_format(string &str){
	return str.find_first_not_of("xy") == std::string::npos;
}
