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
#include <stdlib.h>
#include <sys/wait.h>

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
    ssize_t n;

    while((n = read(0, &actual, 1) > 0)){
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
    int fd;

    if(name == NULL){
        fd = 0;
    } else {
        fd = open(name, O_RDONLY);
    }

    if(fd == -1){
        perror(RED "Error al abrir el archivo." RESET);
    }
    while((readed_bytes = read(fd, &c, 1)) > 0){
        write(1, &c, 1);
    }
    printf("\n");

    if(fd != 0) close(fd);
}

void copyFile(char *source, char *destiny){
    int fd_source, fd_destiny;
    if(destiny == NULL){
        fd_source = 0;
        fd_destiny = open(source, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } else {
        fd_source = open(source, O_RDONLY);
        fd_destiny = open(destiny, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }


    if(fd_source == -1){
        perror(RED "Error al cargar el archivo de origen." RESET);
        // close(fd_source);
        return;
    }


    if(fd_destiny == -1){
        perror(RED "Error al crear el archivo destino." RESET);
//        close(fd_source);
        return;
    }

    char c;
    while(read(fd_source, &c, 1) > 0){
        write(fd_destiny, &c, 1);
    }

    close(fd_destiny);
    if(fd_source != 0){
        close(fd_source);
    }
}

int main(){

    char buffer[256];
    char *commands;
    
    char *command;
    char *arg1;
    char *arg2;

    char *current_pos;
    char *next_pipe;

    int pipe_fds[2];
    int input = 0;

    ssize_t bytes_written, bytes_read;
    int fd;
    int activo = 1;

    while(1){
        printf(BLUE "mi-terminal" RESET ":" GREEN "~$ " RESET);
        if(fgets(buffer, sizeof(buffer), stdin) == NULL) break;
        
        buffer[strcspn(buffer, "\r\n")] = 0; // caracter nulo.

        current_pos = buffer;
        input = 0; // por defecto el teclado.

        while(current_pos != NULL && *current_pos != '\0'){
            next_pipe = strchr(current_pos, '|'); 

            if(next_pipe != NULL){
                *next_pipe = '\0';
            }

            // no considerar espacios
            while (isspace((unsigned char)*current_pos)) current_pos++;

            if(next_pipe != NULL){
                if(pipe(pipe_fds) == -1){
                    perror("ERror al crear pipa.");
                    break;
                }
            }

            char temp[256];
            strcpy(temp, current_pos);
            char *comandoprimero = strtok(temp, " ");

            if(comandoprimero != NULL && strcmp(comandoprimero, "exit") == 0){
                activo = 0;
                break;
            }

            pid_t pid = fork();
            
            if(pid == 0){
                
                // en lugar de leerme de la terminal leeme de la pipa.
                if(input != 0){
                    dup2(input, 0);
                    close(input);
                }
                // 0 lectura y 1 escritura
                if(next_pipe){
                    close(pipe_fds[0]);
                    dup2(pipe_fds[1], 1);
                    close(pipe_fds[1]);
                }

                char *args[255];
                int i = 0;
                char *token = strtok(current_pos, " ");
                while(token != NULL){
                    token[strcspn(token, "\r\n")] = 0;

                    if(token[0] == '"') token++;
                    
                    int len = strlen(token);
                    if(len > 0 && token[len-1] == '"') token[len-1] = '\0';

                    args[i++] = token;
                    token = strtok(NULL, " "); // continua jas
                }
                args[i] = NULL;

                if(args[0] == NULL) exit(0);


                if(strcmp(args[0], "mycat") == 0){
                    readFile(args[1]);
                    exit(0);
                } else if (strcmp(args[0], "mycat>") == 0){
                    if(args[1] != NULL){
                        crearArchivo(args[1]);
                    } else {
                        printf(YELLOW "Error: Se espera un argumento.\n" RESET);

                    }
                    exit(0);
                } else if(strcmp(args[0], "mycp") == 0){
                    arg1 = args[1];
                    arg2 = args[2];
                    
                    copyFile(arg1, arg2);
               
                    exit(0);
                } else if(strcmp(args[0], "remove") == 0){
                    arg1 = args[1];

                    if(arg1 == NULL){
                        printf(YELLOW "Error: Falta el nombre del archivo.\n" RESET);
                    } else {
                        
                        if(unlink(arg1) == 0){
                            printf(GREEN "Archivo '%s' eliminado correctamente.\n" RESET, arg1);
                        } else {
                            perror(RED "Error al intentar eliminar" RESET);
                        }

                    }
                    exit(0);
                }
                else if(strcmp(args[0], "exit") == 0){
                    activo = 0;
                    exit(1);
                } else {
                    execvp(args[0], args);
                    exit(1);
                }


            } else if (pid > 0){
                wait(NULL);

                if(input != 0) close(input);

                if(next_pipe){
                    close(pipe_fds[1]);
                    input = pipe_fds[0];
                } else {
                    input = 0;
                }


            }

            if(next_pipe != NULL){
                current_pos = next_pipe + 1;
            } else {
                current_pos = NULL;
            }

        }
            if(activo == 0){
                    break;
                }
        // command = strtok(buffer, " ");
    
    }

    return 0;
}