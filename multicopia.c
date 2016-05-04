#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char** argv){
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
  int hijo;
  int i = 1+gotF+gotQ+gotR;
  for(; i < argc - 1; i++){
    hijo = fork();
    if(hijo == 0) {
      printf("PPID hijo: %d archivo: %s\n", getpid(), argv[i]);
      if(gotQ == 1){
        char fin[100];
        strcpy(fin, argv[argc-1]);
        strcat(fin, " 2>/dev/null"); //no le gusta
        printf("Destino: %s\n", fin);
        execlp("cp", "cp",argv[i], fin, NULL);
     }

      if(gotF == 1) {
         execlp("cp", "cp", "-f", argv[i], argv[argc-1], NULL);
      } else {
        execlp("cp", "cp", argv[i], argv[argc-1], NULL);
      }
    }
  }
  int juan;
  waitpid(hijo, &juan, 0);
  printf("Papa termino\n");
  return 0;
}
