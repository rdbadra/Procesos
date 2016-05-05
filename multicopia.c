#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char** argv){
  int size = argc;
  printf("PPID: %d\n", getpid());
  int gotF = 0;
  int gotQ = 0;
  int gotR = 0;
  if(strcmp(argv[1], "-f")==0){
    gotF = 1;
  }
  if(strcmp(argv[1], "-q")==0){
    gotQ = 1;
  }
  if(strcmp(argv[1], "-r")==0){
    gotR = 1;
  }
  int arrayHijos[size];
  int i = 1+gotF+gotQ+gotR;
  for(; i < argc - 1; i++){
    arrayHijos[i] = fork();
    if(arrayHijos[i] == 0) {
      printf("PPID hijo: %d archivo: %s\n", getpid(), argv[i]);
      if(gotQ == 1){
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 2);
        close(fd);
        execlp("cp", "cp", argv[i], argv[argc-1], NULL);
     }
      if(gotF == 1) {
        printf("F\n");
        execlp("cp", "cp", "-f", argv[i], argv[argc-1], NULL);
      }
        execlp("cp", "cp", argv[i], argv[argc-1], NULL);
    }
  }
  i = 1+gotF+gotQ+gotR;
  int juan;
  for(; i < argc - 1; i++){
    printf("PID: %d\n", arrayHijos[i]);
    waitpid(arrayHijos[i], &arrayHijos[i], 0);
    printf("Despues PID: %d\n", arrayHijos[i]);
  }


  printf("Papa termino\n");
  return 0;
}
