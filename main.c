#define RED "\x1b[31m"
#define GREEN "\x1b[32m"
#define YELLOW "\x1b[33m"
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

void trim(char *str){
    char *end;

    while(isspace((unsigned char)*str)) str++;

    if(*str == 0) return;

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    end[1] = '\0';
}

void limpiarEntrada(){
    int car = 0;
    while(car != '\n' && car != EOF){
        car = getchar();
    }
}

void crearArchivo(char *nombre){
    int fd = open(nombre, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if(fd == -1){
        perror(RED "Error al crear el archivo." RESET);
        return;
    }

    char actual, siguiente;

    while(read(0, &actual, 1) > 0){
        if(actual == '$'){
            if(read(0, &siguiente, 1) > 0){
                if(siguiente == '\n') break;
                else {
                    write(fd, &actual, 1);
                    write(fd, &siguiente, 1);
                }
            }

        } else {
            write(fd, &actual, 1);
        }
    }

    close(fd);
}

void readFile(char *name){
    char c;
    int readed_bytes;
    int fd = open(name, O_RDONLY);

    if(fd == -1){
        perror(RED "Error al abrir el archivo." RESET);
    }
    while((readed_bytes = read(fd, &c, 1)) > 0){
        write(1, &c, 1);
    }
    printf("\n");

    close(fd);
}

void copyFile(char *source, char *destiny){
    
    int fd_source = open(source, O_RDONLY);
    if (fd_source == -1){
        perror(RED "Error al abrir el archivo fuente" RESET);
        return;
    }

    int fd_destiny = open(destiny, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd_destiny == -1){
        perror(RED "Error al crear el archivo destino." RESET);
        close(fd_source);
        return;
    }

    char c;
    while(read(fd_source, &c, 1) > 0){
        write(fd_destiny, &c, 1);
    }

    close(fd_destiny);
    close(fd_source);
}

int main(){

    char buffer[256];
    char *commands;
    
    char *command;
    char *arg1;
    char *arg2;

    ssize_t bytes_written, bytes_read;
    int fd;

    while(1){
        printf(BLUE "mi-terminal" RESET ":" GREEN "~$ " RESET);
        if(fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        
        buffer[strcspn(buffer, "\n")] = 0; // caracter nulo.

        commands = strtok(buffer, "|");

        while(commands != NULL){
            printf("%s\n", commands);
            commands = strtok(NULL, commands);

            command = commands;
            trim(command);
            command = strtok(command, " ");
            printf("%s", command);
        }
        
        command = strtok(buffer, " ");

        if(command == NULL) continue;

        if(strcmp(command, "mycat") == 0){
            arg1 = strtok(NULL, " ");
            if(arg1 != NULL){
                readFile(arg1);
            } else {
                printf(YELLOW "Error: Se espera un argumento.\n" RESET);
            }
        } else if (strcmp(command, "mycat>") == 0){
            arg1 = strtok(NULL, " ");
            if(arg1 != NULL){
                crearArchivo(arg1);
            } else {
                printf(YELLOW "Error: Se espera un argumento.\n" RESET);

            }
        } else if(strcmp(command, "mycp") == 0){
            arg1 = strtok(NULL, " ");
            arg2 = strtok(NULL, " ");
            
            if(arg1 != NULL && arg2 != NULL){
              copyFile(arg1, arg2);
            }else {
                printf(YELLOW "Error: Se esperan dos argumentos, origen y destino.\n" RESET);
            }

        } else if(strcmp(command, "remove") == 0){
            arg1 = strtok(NULL, " ");

            if(arg1 == NULL){
                printf(YELLOW "Error: Falta el nombre del archivo.\n" RESET);
            } else {
                
                if(unlink(arg1) == 0){
                    printf(GREEN "Archivo '%s' eliminado correctamente.\n" RESET, arg1);
                } else {
                    perror(RED "Error al intentar eliminar" RESET);
                }

            }
        }
        else if(strcmp(command, "exit") == 0){
            break;
        }
    }

    return 0;
}