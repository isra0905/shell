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
    char hostname[_SC_HOST_NAME_MAX + 1];
    char **command = NULL;
    int exit = 1;
    char *colorUser = ANSI_COLOR_GREEN;
    char *colorPath = ANSI_COLOR_BLUE;
    char *colorAux;
    int aux;
    do
    {
        if (getcwd(directory, sizeof(directory)) != NULL && gethostname(hostname, _SC_HOST_NAME_MAX + 1) == 0 && (username = getlogin()) != NULL)
        {
            printf("%s%s@%s" ANSI_COLOR_RESET ":", colorUser, username, hostname);
            printf("%s%s" ANSI_COLOR_RESET "> ", colorPath, directory);
            line = readLine();
            command = tokenize(line);

            if (isEmpty(line) == 0)
            {
                if ((exit = strncmp(line, "exit", 5)) == 0)
                    ;
                else if (strncmp(command[0], "cd", 3) == 0)
                {
                    if (isEmpty(command[1]) == 0)
                    {
                        if (chdir(command[1]) != 0)
                        {
                            perror("error changing directory");
                        }
                    }
                }
                else if (strncmp(command[0], "echo", 5) == 0)
                {
                    if (isEmpty(command[1]) == 0)
                    {
                        if ((aux = presentRedirection1(command)) != -1)
                        {
                            processRedirection1(aux, command, line);
                        }
                        else if ((aux = presentRedirection2(command)) != -1)
                        {
                            processRedirection2(aux, command, line);
                        }
                        else
                        {
                            printEcho(command, line, 0);
                        }
                    }
                }
                else if (strncmp(command[0], "color", 6) == 0)
                {
                    if (isEmpty(command[1]) == 0)
                    {
                        if (strncmp(command[1], "help", 5) == 0)
                        {
                            printf("Usage: color -[argument] [color]\n\nThe possible arguments are:\n-u: User and host color\n-p: Path color");
                            printf("\n\nAvailable colors are: red, green, yellow, blue, purple, cyan and white\n\nColors and arguments must be written in lowercase.\n");
                        }
                        else if (strncmp(command[1], "-u", 3) == 0)
                        {
                            if (isEmpty(command[2]) == 0)
                            {
                                colorAux = selectColor(command[2]);
                                if (colorAux != NULL)
                                {
                                    colorUser = colorAux;
                                }
                            }
                        }
                        else if (strncmp(command[1], "-p", 3) == 0)
                        {
                            if (isEmpty(command[2]) == 0)
                            {
                                colorAux = selectColor(command[2]);
                                if (colorAux != NULL)
                                {
                                    colorPath = colorAux;
                                }
                            }
                        }
                    }
                    else
                    {
                        printf("Try using: color help\n");
                    }
                }
                else if ((aux = presentPipe(command)) != -1)
                {
                    processPipe(aux, command);
                }
                else if ((aux = presentRedirection1(command)) != -1)
                {
                    processRedirection1(aux, command, line);
                }
                else if ((aux = presentRedirection2(command)) != -1)
                {
                    processRedirection2(aux, command, line);
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
            }
        }
        else
        {
            perror("error initializing variables");
            return 1;
        }

        freeMemory(command);
        free(line);
        command = NULL;
        line = NULL;

    } while (exit != 0);

    return 0;
}