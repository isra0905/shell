#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define ANSI_COLOR_GREEN "\e[0;32m"
#define ANSI_COLOR_BLUE "\e[0;34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define PATH_MAX 4096

int main()
{
    char *line = NULL;
    char directory[PATH_MAX];
    char *username;
    char *username2;
    char hostname[_SC_HOST_NAME_MAX + 1];
    char **command;
    int exit = 1;
    char *colorUser = ANSI_COLOR_GREEN;
    char *colorPath = ANSI_COLOR_BLUE;
    char *colorAux;
    int aux;
    do
    {
        // if (getcwd(directory, sizeof(directory)) != NULL && gethostname(hostname, _SC_HOST_NAME_MAX + 1) == 0 && (username = getlogin()) != NULL)
        if (1 == 1)
        {
            username = "Paco";
            username2 = "Pedro";
            printf("%s%s@%s" ANSI_COLOR_RESET ":", colorUser, username, username2);
            printf("%s%s" ANSI_COLOR_RESET "> ", colorPath, directory);
            line = readLine();
            command = tokenize(line);

            if (isEmpty(line) == 0)
            {
                if (strncmp(command[0], "cd", 3) == 0)
                {
                    if (chdir(command[1]) != 0)
                    {
                        perror("error changing directory");
                    }
                }
                else if (strncmp(command[0], "echo", 5) == 0)
                {
                    if(command[1] != '\0') printf("%s\n", command[1]);
                }
                else if (strncmp(command[0], "color", 6) == 0)
                {
                    if(strncmp(command[1], "help", 5) == 0){
                        printf("Usage: color -[argument] [color]\n\nThe possible arguments are:\n-u: User and host color\n-p: Path color");
                        printf("\n\nAvailable colors are: red, green, yellow, blue, purple, cyan and white\n\nColors and arguments must be written in lowercase.");
                    }else if (strncmp(command[1], "-u", 3) == 0)
                    {
                        if(command[2] != '\0'){
                            colorAux = selectColor(command[2]);
                            if (colorAux != NULL)
                            {
                                colorUser = colorAux;
                            }
                            
                        }
                    }else if (strncmp(command[1], "-p", 3) == 0)
                    {
                        if(command[2] != '\0'){
                            colorAux = selectColor(command[2]);
                            if (colorAux != NULL)
                            {
                                colorPath = colorAux;
                            }
                        } 
                    }
                    free(colorAux);
                }
                else if ((aux = presentPipe(command)) != -1)
                {
                    processPipe(aux, command);
                }
                else
                {
                    pid_t pid = fork();

                    if (pid == 0)
                    {
                        execvp(command[0], command);
                    }
                    else if (pid > 0)
                    {
                        wait(NULL);
                    }
                    else if (pid < 0)
                    {
                        perror("fork failed");
                    }
                }

                exit = strncmp(line, "exit", 5);
            }
        }
        else
        {
            perror("error initializing variables");
            return 1;
        }

        free(command);
        free(line);

    } while (exit != 0);

    return 0;
}