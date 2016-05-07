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

int waitAll(int* arrayHijos){
  int i = start();
  for(; i < size-1; i++){
    waitpid(arrayHijos[i], &arrayHijos[i], 0);
  }
  return 0;
}

int copiar(char* file, char* destino){
  int i = start();
  for(; i < size - 1; i++){
      if(gotQ == 1){
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 2);
        close(fd);
        execlp("cp", "cp", file, destino, NULL);
     }
      if(gotF == 1) {
        execlp("cp", "cp", "-f", file, destino, NULL);
      }
        execlp("cp", "cp", file, destino, NULL);
    }
  return 0;
}

int lanzarErrores(int* arrayHijos, int argc, char** argv){
  int i = start();
  for(;i < size -1 ; i++){
    if(arrayHijos[i] != 0){
      arrayHijos[i] = fork();
      if(arrayHijos[i] == 0) copiar(argv[i], argv[argc-1]);
    }
  }
  i = start();
  for(; i < size - 1; i++){
    waitAll(arrayHijos);
  }
  return 0;
}

void clear(){
    while (getchar() != '\n');
}

int error(){
  printf("Se ha producido un error, ¿Quiere reintentarlo?[s, n]\n");
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
    waitAll(arrayHijos);
  }

  if(gotR == 1){
    i = start();
    for(; i < argc - 1; i++){
      if(arrayHijos[i] != 0){
        if(error()==1){
          lanzarErrores(arrayHijos, argc, argv);
          i = start();
        } else {
          return -1;
        }
      }
    }
  }
  return 0;
}
