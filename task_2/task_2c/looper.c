#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int signum){
	signal(signum, handler);
	char* str_sig = strsignal(signum);
	printf("received signal: %d name: %s\n", signum, str_sig);
	signal(signum, SIG_DFL);
	raise(signum);
}
int main(int argc, char **argv){
	signal(SIGTSTP, handler);
	signal(SIGINT, handler);
	signal(SIGCONT, handler);
	printf("Starting the program\n");

	while(1) {
		sleep(2);
	}

	return 0;
}
