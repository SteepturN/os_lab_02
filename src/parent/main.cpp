#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <cstdlib>
#include <charconv>
#include <string>
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
const char error_creating_pipe[] = "can't create pipe\n";
const char error_child_args[] = "num of descriptor is too large\n";
const char error_creating_process[] = "can't create another process\n";
const char error_executing_program[] = "can't exec prog\n";
const char error_reading_input[] = "error reading from input\n";
const char error_writing_pipe[] = "error writing to pipe\n";
const char error_reading_pipe[] = "error reading from pipe\n";
//char buffer[200] = { 0 };

int read_num(int& num);
int main() {
	const int filename_length = 40;
	char filename[filename_length];
	//int last_func_error_code;
	for(int i = 0; i < filename_length; i++) {
		if(read(STDIN, filename+i, sizeof(char)) == -1) {
			if(write(STDERR, error_reading_input, sizeof(error_reading_input)) == -1) {
				return ERROR_WRITING_ERR;
			}
			return ERROR_READING_INPUT;
		}
		if(((filename[i] < 'a') || (filename[i] > 'z')) && ((filename[i] < 'A') || (filename[i] > 'Z')) && (filename[i]!='/') && \
		(filename[i] != '.') && (filename[i] != '_')) {
			filename[i] = '\0';
			break;
		}
	}
	//std::string&& error_opening_file = static_cast<std::string>("can't execute prog ") + static_cast<std::string>(filename);
	int number = 1; //>0
	bool all_ok = true;

	int pipe_parent_to_son[2];//
	char pipe_son_reading[20] = { 0 };

	int pipe_son_to_parent[2];
	char pipe_son_writing[20] = { 0 };

	if(pipe(pipe_parent_to_son)==-1) {//
		if(write(STDERR, error_creating_pipe, sizeof(error_creating_pipe)) == -1) {
			return ERROR_WRITING_ERR;
		}
		return ERROR_CREATING_PIPE;
	}
	if(pipe(pipe_son_to_parent)==-1) {//
		if(write(STDERR, error_creating_pipe, sizeof(error_creating_pipe)) == -1) {
			return ERROR_WRITING_ERR;
		}
		return ERROR_CREATING_PIPE;//
	}
	if(std::to_chars(pipe_son_reading, pipe_son_reading+20, pipe_parent_to_son[READ_END]).ec == std::errc::value_too_large) {
		if(write(STDERR, error_child_args, sizeof(error_child_args)) == -1) {
			return ERROR_WRITING_ERR;
		}
		return ERROR_CHILD_ARGS;
	}
	//locale-independent - what does it mean?
	if(std::to_chars(pipe_son_writing, pipe_son_writing+20, pipe_son_to_parent[WRITE_END]).ec == std::errc::value_too_large) {
		if(write(STDERR, error_child_args, sizeof(error_child_args)) == -1) {
			return ERROR_WRITING_ERR;
		}
		return ERROR_CHILD_ARGS;
	}
	//std::cout <<pipe_parent_to_son[READ_END] << " = " << pipe_son_reading << ' ' << pipe_son_to_parent[WRITE_END] << " = " << pipe_son_writing << std::endl;
	pid_t pid = fork();
	if(pid < 0) { //
		if(write(STDERR, error_creating_process, sizeof(error_creating_process)) == -1) {
			return ERROR_WRITING_ERR;
		}
		return ERROR_CREATING_PROCESS;//
	} else if(pid == 0) {//
		//std::cout << "from child: child pid:" << getpid() << std::endl;//
		close(pipe_parent_to_son[WRITE_END]); //errors?
		close(pipe_son_to_parent[READ_END]); // why doesn't pipe close everywhere?
		if(execl(filename, pipe_son_reading, pipe_son_writing, static_cast<char*>(NULL)) == -1) {// if I give pointer, would it work?
			if(write(STDERR, error_executing_program, sizeof(error_executing_program)) == -1) {
				return ERROR_WRITING_ERR;
			}
			return ERROR_EXECUTING_PROGRAM;
		}
	} else {
		close(pipe_parent_to_son[READ_END]);
		close(pipe_son_to_parent[WRITE_END]);
		//std::cout << "from parent: child pid" << pid << std::endl;
	}
	//FILE write_nums_here = fdopen(pipe_parent_to_son[1], "w"); - it's wrong but why?
	while(all_ok) {
		read_num(number);
		if(write(pipe_parent_to_son[WRITE_END], &number, sizeof(number)) == -1) {
			if(write(STDERR, error_writing_pipe, sizeof(error_writing_pipe)) == -1) {
				return ERROR_WRITING_ERR;
			}
			return ERROR_WRITING_PIPE;
		} else if(read(pipe_son_to_parent[READ_END], &all_ok, sizeof(all_ok)) == -1) {
			if(write(STDERR, error_reading_pipe, sizeof(error_reading_pipe)) == -1) {
				return ERROR_WRITING_ERR;
			}
			return ERROR_READING_PIPE;
		}
	}
	return ALL_GOOD;
}

int read_num(int& num) {
	int sign = 1;
	num = 0;
	char num_ch = 0;
	bool go_on = true;
	while(true) {
		if(read(STDIN, &num_ch, sizeof(num_ch)) == -1) {
			if(write(STDERR, error_reading_input, sizeof(error_reading_input)) == -1) {
				return -1;//
			}
			return -1;//
		}
		if(num_ch == '-') {
			sign = -1;
			continue;
		}
		if((num_ch >= '0') && (num_ch <= '9')) break;
		sign = 1;
	}
	while(go_on) {
		num=num*10+(num_ch-'0');
		if(read(STDIN, &num_ch, sizeof(num_ch)) == -1) {
			if(write(STDERR, error_reading_input, sizeof(error_reading_input)) == -1) {
				return -1;//
			}
			return -1;//
		}
		if((num_ch < '0') || (num_ch > '9')) break;
	}
	num*=sign;
	return 0;
}

