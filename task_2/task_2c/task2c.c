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

void updateProcessList(process **process_list);

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

void freeSingleProcess(process * proc){
  freeCmdLines(proc->cmd);
  proc->next = NULL;
  free(proc);
}

void printProcessList(process ** process_list){
  if(!process_list) return;

  updateProcessList(process_list);
	process * curr = *process_list;
  process * prev = curr;

  //TERMINATED leading procceses;
  while(curr && curr->status == TERMINATED){
    printf("%d %s %s\n", curr->pid, curr->cmd->arguments[0], "TERMINATED");
    prev = curr->next;
    freeSingleProcess(curr);
    curr = prev;
    shell_process_list = curr; //updating head of list
  }

  if(!curr) return; //all process in list were TERMINATED
  printf("%d %s %s\n", curr->pid, curr->cmd->arguments[0], curr->status == TERMINATED ? "TERMINATED" :
                                                              curr->status == SUSPENDED ? "SUSPENDED" : "RUNNING");
  curr = curr->next;

	while(curr){
		printf("%d %s %s\n", curr->pid, curr->cmd->arguments[0], curr->status == TERMINATED ? "TERMINATED" :
                                                                curr->status == SUSPENDED ? "SUSPENDED" : "RUNNING");

    if(curr->status == TERMINATED){
      prev->next = curr->next;
      freeSingleProcess(curr);
      curr = prev->next;
    } else {
      prev = curr;
      curr = curr->next;
    }
	}
}

void freeProcessList(process* process_list){
  process * runner = process_list;
  while(runner){
    process *next = runner->next;
    freeCmdLines(runner->cmd);
    runner->next = NULL;
    free(runner);
    runner = next;
  }
}

void updateProcessStatus(process* process_list, int pid, int status){
  if(!process_list) return;

  process * runner = process_list;

  while(runner){
      if(runner->pid == pid){
        runner->status = status;
        break;
      }
     runner = runner->next;
  }
}

void updateProcessList(process **process_list){
  if(!process_list) return;

  process * runner = *process_list;

  while(runner){
     int status;
     pid_t child_pid = waitpid(runner->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
     if(child_pid == runner->pid){
       if(WIFEXITED(status) || WIFSIGNALED(status)){
         runner->status = TERMINATED;
       } else if(WIFSTOPPED(status)){
         runner->status = SUSPENDED;
       } else {
         runner->status = RUNNING;
       }
     }
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
      if(cmd_line->blocking){
        waitpid(pid, &status, WUNTRACED);
        updateProcessStatus(shell_process_list, pid, TERMINATED);
      }

			return status;
		} else if(execvp(cmd_line->arguments[0], cmd_line->arguments) < 0){
			//child process
			perror("error executing command\n");
			_exit(1);
		}
	}
}

void nap(int nap_pid, int num_of_seconds){
  int pid;
  if(!(pid = fork())){
    int result = kill(nap_pid, SIGSTOP);
    if(result < 0){
      fprintf(stderr, "stopping %d failed\n", nap_pid);
      _exit(1);
    }

    sleep(num_of_seconds);
    if(result == 0){
      result = kill(nap_pid, SIGCONT);
    }

    if(result < 0){
      fprintf(stderr, "continue %d failed\n", nap_pid);
    }
    _exit(1);
  }
}

void stop(int pid_to_kill){
  int pid;
  if(!(pid = fork())){
    int result = kill(pid_to_kill, SIGINT);
    if(result < 0){
      fprintf(stderr, "killing %d failed\n", pid_to_kill);
    }
    _exit(1);
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
				puts("procces List is empty");
			}
      freeCmdLines(cmd_line);
		} else if(strcmp(cmd_line->arguments[0], "nap") == 0){
      nap(atoi(cmd_line->arguments[2]), atoi(cmd_line->arguments[1]));
      freeCmdLines(cmd_line);
    } else if(strcmp(cmd_line->arguments[0], "stop") == 0){
      stop(atoi(cmd_line->arguments[1]));
      freeCmdLines(cmd_line);
    } else {
			execute(cmd_line);
		}

		sleep(2);
	}

  freeProcessList(shell_process_list);
	return 0;
}
