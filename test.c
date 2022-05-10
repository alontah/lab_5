#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
  int pid = fork();
  int status;
  if(pid){
    pid_t tpid = wait(&status);
    if(WIFEXITED(status)){
      printf("exit status of %ld : %d\n", (long)tpid, WEXITSTATUS(status));
    }
  } else {
     _exit(12);
  }
  return 0;
}
