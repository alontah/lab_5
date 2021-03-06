#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "LineParser.h"

#define BUFFER_SIZE 2048

int execute(cmdLine *cmd_line){
	if(cmd_line){
		int pid = fork();
		int status;

		if(pid){
			waitpid(pid, &status, (1 - cmd_line->blocking) | WUNTRACED);
			freeCmdLines(cmd_line);
			return status;
		} else {
			if(execvp(cmd_line->arguments[0], cmd_line->arguments) < 0){
				perror("err\n");
				_exit(1);
			}
		}
	}
}

int main(int argc, char **argv){
	int debug = 0;
	char curr_path_buff[BUFFER_SIZE];
	char user_buff[BUFFER_SIZE];
	printf("Starting the program\n");

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-D") == 0){
			debug = 1;
		}
	}

	while(1) {
		getcwd(curr_path_buff, BUFFER_SIZE);
		printf("%s\n",curr_path_buff);
		fgets(user_buff, BUFFER_SIZE, stdin);
		cmdLine *cmd_line = parseCmdLines(user_buff);
		if(strcmp(cmd_line->arguments[0], "quit") == 0){
			freeCmdLines(cmd_line);
			break;
		}
		execute(cmd_line);
		sleep(2);
	}

	return 0;
}
