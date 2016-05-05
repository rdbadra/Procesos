#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int gotF = 0;
int gotQ = 0;
int gotR = 0;
int size;

int start(){
  int i = 1+gotF+gotQ+gotR;
  return i;
}

void clear(){
    while (getchar() != '\n');
}

int copiar(char* file, char* destino){
  //printf("PPID: %d\n", getpid());
  int i = start();
  for(; i < size - 1; i++){
      printf("PPID hijo: %d archivo: %s\n", getpid(), file);
      if(gotQ == 1){
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 2);
        close(fd);
        execlp("cp", "cp", file, destino, NULL);
     }
      if(gotF == 1) {
        printf("F\n");
        execlp("cp", "cp", "-f", file, destino, NULL);
      }
        execlp("cp", "cp", file, destino, NULL);
    }
  return 0;
}

int error(char* file){
  printf("el fichero %s no se ha copiado, ¿Quiere reintentarlo?[s, n]\n", file);
  char yes;
  yes = getchar();
  if(yes == 's'){
      clear();
      return 1;
  } else if(yes == 'n'){
      clear();
      return -1;
  } else {
      printf("el caracter introducido no es valido\n");
      clear();
      return -1;
  }

  return 0;
}

int main(int argc, char** argv){
  size = argc;
  int arrayHijos[size];
  printf("PPID: %d\n", getpid());
  if(strcmp(argv[1], "-f")==0){
    gotF = 1;
  }
  if(strcmp(argv[1], "-q")==0){
    gotQ = 1;
  }
  if(strcmp(argv[1], "-r")==0){
    gotR = 1;
  }
  int i = start();
  for(;i < argc-1; i++){
    arrayHijos[i] = fork();
    if(arrayHijos[i]==0)copiar(argv[i], argv[argc-1]);
  }

  i = start();
  for(; i < argc - 1; i++){
    printf("PID: %d\n", arrayHijos[i]);
    waitpid(arrayHijos[i], &arrayHijos[i], 0);
    printf("Despues PID: %d\n", arrayHijos[i]);
  }
  //FALTA HACER QUE ENVIE TODAS LAS FALLIDAS DE NUEVO
  if(gotR == 1){
    i = start();
    for(; i < argc - 1; i++){
        while(arrayHijos[i]!=0){
          if(error(argv[i])==-1){
            arrayHijos[i] = 0;
            break;
          } else {
            arrayHijos[i] = fork();
            if(arrayHijos[i]==0)copiar(argv[i], argv[argc-1]);
          }
          waitpid(arrayHijos[i], &arrayHijos[i], 0);
        }
      }
    }
  return 0;
}
