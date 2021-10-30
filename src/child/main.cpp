#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <set>
#include <cmath>
#include <charconv>
#include <sys/stat.h>
#include <fcntl.h>
enum {
	READ_END=0,
	WRITE_END=1,
};
enum {
	STDIN = 0,
	STDOUT = 1,
	STDERR = 2,
};
enum {
	ALL_GOOD=0,
	ERROR_CREATING_PIPE,
	ERROR_CHILD_ARGS,
	ERROR_CREATING_PROCESS,
	ERROR_EXECUTING_PROGRAM,
	ERROR_READING_INPUT,
	ERROR_WRITING_PIPE,
	ERROR_READING_PIPE,
	ERROR_WRITING_ERR,
};

bool is_prime(const int& number);
void put_all_primes_until(std::set<int>& primes, int num);

int main(int argc, char* argv[]) {
	const char space = ' ';
	const char error_not_enough_arguments[] = "not enough arguments";
	const char error_reading_pipe[] = "child: error with reading";
	const char error_writing_pipe[] = "child: error with writing";
	if(argc < 2) {
		if(write(STDERR, error_not_enough_arguments, sizeof(error_not_enough_arguments)) == -1) {
			return ERROR_WRITING_ERR;
		}
		for(int i = 0; i < argc; i++)
			if(write(STDERR, argv[i], sizeof(argv[i])) == -1) {
				return ERROR_WRITING_ERR;
			}
		return 1; //hz pochemy ne 3 argumenta kak pri vizove iz kommandnoi stroki
	}
	int&& pipe_parent_to_son_read = std::stoi(static_cast<std::string>(argv[0])); //I can use try-catch, but naaah
	int&& pipe_son_to_parent_write = std::stoi(static_cast<std::string>(argv[1]));
	int file_descriptor = open("numbers.ans", O_CREAT | O_TRUNC | O_WRONLY);
	int number = 1;// kst kak vishlo v proshlii raz, esli negative opredelalsa vne while
	char number_string[10] = { 0 };
	int num_size_to_write = 0;
	while(true) {
		if(read(pipe_parent_to_son_read, &number, sizeof(number)) == -1) {
			if(write(STDERR, error_reading_pipe, sizeof(error_reading_pipe)) == -1) {
				return ERROR_WRITING_ERR;
			}
			return ERROR_READING_PIPE;
		}
		//std::cerr << number << ' ';
		bool negative = number < 0;
		bool prime = false;
		if(!negative) {
			prime = is_prime(number);
			//magic __ prime=
		}
		bool num_is_correct = !(negative || prime);
		if(write(pipe_son_to_parent_write, &num_is_correct, sizeof(num_is_correct)) == -1) {
			if(write(STDERR, error_writing_pipe, sizeof(error_writing_pipe)) == -1) {
				return ERROR_WRITING_ERR;
			}
			return ERROR_WRITING_PIPE;
		}
		if(num_is_correct) {
			for(long unsigned int i = 0; i < sizeof(number_string); i++) number_string[i] = '\0';
			std::to_chars(number_string, number_string + sizeof(number_string), number);
			//std::cerr << number_string << ' ';
			for(long unsigned int i = 0; i < sizeof(number_string); ++i)
				if(number_string[i] == '\0') { num_size_to_write = i; break;} //I can div 10
			write(file_descriptor, number_string, num_size_to_write);
			write(file_descriptor, &space, sizeof(' '));
		}
		else return ALL_GOOD;
	}
	return ALL_GOOD;
}

bool is_prime(const int& number) {
	static std::set<int> primes{2, 3, 5, 7, 11, 13};
	if(number < 2) return false;
	if(primes.count(number)) return true;
	//int min_n = min(std::trunc(std::sqrt(number)), *primes.rbegin());
	if(std::trunc(std::sqrt(number)) > *primes.rbegin())
		put_all_primes_until(primes, std::trunc(std::sqrt(number)));
	for(auto another_prime = primes.begin(); another_prime != primes.end(); another_prime++)
		if(number % (*another_prime )== 0) return false;
	primes.insert(number); //
	return true;
} //make declarations of functions in the top of the prog

void put_all_primes_until(std::set<int>& primes, int num) {
	int max_exist = *(primes.rbegin());
	if(max_exist > num) return;

	for(int i = (max_exist - max_exist%6)+6; i <= num; i+=6) {
		is_prime(i-1);
		is_prime(i+1); //it pushes them for u
	}
}

