#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

/*Funcion para comparar si el 
fichero de origen y destino resultan 
ser el mismo*/
int compareFiles(char* path1, char* path2){
    struct stat buf1;
    struct stat buf2;
    int status1 = stat(path1, &buf1);
    int status2 = stat(path2, &buf2);
    if(status1 < 0){
        perror("status1 comp");
        return -1;
    }
    if(status2 < 0){
        perror("status2 comp");
        return -1;
    }
    if(buf1.st_dev == buf2.st_dev && buf1.st_ino == buf2.st_ino){
        printf("son el mismo archivo\n");
        return -1;
    } 
    return 0;
}

/*Funcion para cambiar los permisos del segundo
archivo por los del primero*/
int permission(char* sourcePath, char* path2){
    struct stat buf1;
    struct stat buf2;
    int status1 = stat(sourcePath, &buf1);
    int status2 = stat(path2, &buf2);
    if(status1 < 0){
        perror("status1");
        return -1;
    }
    if(status2 < 0){
        perror("status2");
        return -1;
    }

    if(chmod(path2, buf1.st_mode)<0){
        perror("chmod");
    }
    return 0;
}

/*Funcion que te devuelve el nombre del archivo 
sin la ruta*/
char* findName(char* path, int pos, char* name ){
    if(pos == -1){
        return path;
    }
    pos++;
    
    int i = 0;
    while(pos < strlen(path)){
        name[i] = path[pos];
        pos++;
        i++;
    }
    name[i] = '\0';
    return name;
}

/*Funcion que comprueba si la ruta 
acaba con slash*/
int directoryEnd(char* directory){
    int size = strlen(directory);
    char fin = directory[size-1];
    if(fin == '/'){
        return 0;
    }
    return -1;
}

/*Funcion que devuelve la posicion 
del ultimo slash de la ruta */
int searchSlash(char* path){
    if(strstr(path, "/")==0){
        return -1;
    }
    int i = strlen(path)-1;
    while(path[i] != '/'){
        i--;
    }
    return i;
}

/*Funcion que espera a que el buffer este vacio*/
void clear(){
    while (getchar() != '\n');
}

/*Funcion que copia un archivo en el directorio destino*/
int copy(char* sourcePath, char* directory, int hasF){
    char name[strlen(sourcePath)-searchSlash(sourcePath)];
    char dirSource[strlen(sourcePath)-strlen(name)];
    char* path = findName(sourcePath, searchSlash(sourcePath), name);
    int sizePath = strlen(path);
    char finalDirectory[sizePath+strlen(directory)];
    strcpy(finalDirectory, directory);
    if(directoryEnd(finalDirectory)==-1){
        strcat(finalDirectory, "/");
    }
    strcat(finalDirectory, path);
    
    int fd1 = open(sourcePath, O_RDONLY);
    if(fd1 == -1){
        perror("open fd1");
        return -1;
    }
    int fd2 = open(finalDirectory, O_WRONLY);
    if(hasF == 0){
        if(fd2 != -1){
        if(compareFiles(sourcePath, finalDirectory)==-1){
            return -1;
        }
        printf("el fichero existe, ¿Quiere sobrescribirlo?[s, n]\n");
        char yes;
        yes = getchar();
        if(yes == 's'){
            fd2 = open(finalDirectory, O_WRONLY | O_TRUNC);
            clear();
        } else if(yes == 'n'){
            clear();
            return -1;
        } else {
            printf("el caracter introducido no es valido\n");
            clear();
            return -1;
        }
    }
    }
    if(fd2 == -1){
        fd2 = creat(finalDirectory, S_IRWXU);
        if(fd2 == -1){
            perror("creat");
            return -1;
        }
        permission(sourcePath, finalDirectory);
        fd2 = open(finalDirectory, O_WRONLY);
        permission(sourcePath, finalDirectory);
        if(fd2 == -1){
            perror("open fd2");
            close(fd1);
            return -1;
        }
    }
    int sizePage = 1024;
	char buf[sizePage];
    int numBytes = read(fd1, buf, sizePage);
    write(fd2, buf, numBytes);

	while(numBytes == 1024){
		numBytes = read(fd1, buf, sizePage);

		write(fd2, buf, numBytes);
	}
    close(fd1);
    close(fd2);
    return 0;
}

/*Funcion que comprueba la existencia de -f*/
int searchF(int count, char** argv){
    if(strcmp("-f", argv[1])==0) return 1;
    return 0;
}

/*Funcion que comprueba que los argumentos 
pasados al programa sean correctos*/
int checkArguments(int argc, char** argv){
    if(argc < 3){
        perror("error de argumentos");
        return -1;
    }
    struct stat dir;
    stat(argv[argc-1], &dir);
    if(S_ISDIR(dir.st_mode)==0){
        printf("¡El destino no es un directorio!\n");
        return -1;
    }
    int hasF = searchF(argc, argv);
    if(hasF == 1){
        int i;
        for(i=2; i < argc -1; i++){
            copy(argv[i], argv[argc-1], hasF);
        }
        return 0;
    }
    int i;
    for(i=1; i < argc -1; i++){
        copy(argv[i], argv[argc-1], hasF);
    }
    return 0;
}

int main(int argc, char** argv){
   return checkArguments(argc, argv);
}
