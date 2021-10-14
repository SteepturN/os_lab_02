#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <string>
#include <fstream>
#include <algorithm>
#include <set>
#include <cmath>

bool is_prime(const int& number);
void put_all_primes_until(std::set<int>& primes, int num);

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cerr << "not enough arguments: " << argc << std::endl << argv[0] << std::endl<< argv[1];
		return 1; //hz pochemy ne 3 argumenta kak pri vizove iz kommandnoi stroki
	}
	int&& pipe_parent_to_son_read = std::stoi(static_cast<std::string>(argv[0])); //I can use try-catch, but naaah
	int&& pipe_son_to_parent_write = std::stoi(static_cast<std::string>(argv[1]));
	std::ofstream file("numbers.ans", std::ios::trunc);
	int number(1);// kst kak vishlo v proshlii raz, esli negative opredelalsa vne while
	while(true) {
		if(read(pipe_parent_to_son_read, &number, sizeof(number)) == -1) {
			std::cerr << "child: error with reading" << std::endl;
			return -1;
		}
		//std::cout << number
		bool negative = number < 0;
		bool prime = false;
		if(!negative) {
			prime = is_prime(number);
			//magic __ prime=
		}
		bool num_is_correct = !(prime || negative);
		if(write(pipe_son_to_parent_write, &num_is_correct, sizeof(num_is_correct)) == -1) {
			std::cerr << "child: error with writing" << std::endl;
			return -1;
		}
		if(num_is_correct)
			file << number << " ";
		else return 0;
	}
	return 0;
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






























