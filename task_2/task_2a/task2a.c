#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "LineParser.h"

#define BUFFER_SIZE 2048
#define TERMINATED -1
#define RUNNING 1
#define SUSPENDED 0


typedef struct process{
 cmdLine* cmd; /* the parsed command line*/
 pid_t pid; /* the process id that is
running the command*/
 int status; /* status of the process:
RUNNING/SUSPENDED/TERMINATED */
 struct process *next; /* next process in chain */
} process;

int debug = 0;
process * shell_process_list = NULL;

void handler(int signum){
	signal(signum, handler);
	char *str_sig = strsignal(signum);

	printf("received signal: %d name: %s\n", signum, str_sig);
	signal(signum, SIG_DFL);
	raise(signum);
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
	process * p_to_add = (process*)malloc(sizeof(process));
	p_to_add->cmd = cmd;
	p_to_add->pid = pid;
	p_to_add->status = RUNNING;
	if(process_list){
		p_to_add->next = *process_list;
	} else {
		p_to_add->next = NULL;
	}
	// *process_list ? p_to_add->next = *process_list : NULL;
	*process_list = p_to_add;

}

void printProcessList(process ** process_list){
	process * runner = *process_list;
	while(runner){
		printf("%d %s %s\n", runner->pid, runner->cmd->arguments[0], runner->status == TERMINATED ? "TERMINATED" :
	 																																			runner->status == SUSPENDED ? "SUSPENDED" : "RUNNING");
		runner = runner->next;
	}
}

int execute(cmdLine *cmd_line){
	if(cmd_line){
		int pid = fork();
		int status;

		if(pid){
			//parent process
			if(debug){
				fprintf(stderr, "PID num: %d, Executing command: %s\n", pid, cmd_line->arguments[0]);
			};
			addProcess(&shell_process_list, cmd_line, pid);
			waitpid(pid, &status, (1 - cmd_line->blocking) | WUNTRACED);
			return status;
		} else if(execvp(cmd_line->arguments[0], cmd_line->arguments) < 0){
			//child process
			perror("err\n");
			_exit(1);
		}
	}
}

int main(int argc, char **argv){
	signal(SIGTSTP, handler);
	signal(SIGINT, handler);
	signal(SIGCONT, handler);
	char curr_path_buff[BUFFER_SIZE];
	char user_buff[BUFFER_SIZE];
	printf("Starting the program\n");

	for(int i = 0; i < argc; i++){
		if(strcmp(argv[i], "-d") == 0){
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
		} else if(strcmp(cmd_line->arguments[0], "cd") == 0){
			if(cmd_line -> argCount < 2 && debug){
				fprintf(stderr, "no path entered\n");
			} else if(chdir(cmd_line->arguments[1]) < 0 && debug) {
				fprintf(stderr, "path: %s not found\n", cmd_line->arguments[1]);
			}
			freeCmdLines(cmd_line);
		} else if(strcmp(cmd_line->arguments[0], "showprocs") == 0){
			if(shell_process_list){
				printProcessList(&shell_process_list);
			} else {
				puts("proccesList empty");
			}
		} else {
			execute(cmd_line);
		}

		sleep(2);
	}

	return 0;
}
