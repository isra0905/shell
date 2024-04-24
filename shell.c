#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils.h"

#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_RESET "\x1b[0m"

#define PATH_MAX 4096

int main()
{
    char *line = NULL;
    char directory[PATH_MAX];
    char *username;
    char hostname[_SC_HOST_NAME_MAX + 1];
    char **command;
    int exit;
    do
    {
        if (getcwd(directory, sizeof(directory)) != NULL && gethostname(hostname, _SC_HOST_NAME_MAX + 1) == 0 && (username = getlogin()) != NULL)
        {
            printf(ANSI_COLOR_GREEN "%s@%s" ANSI_COLOR_RESET ":", username, hostname);
            printf(ANSI_COLOR_BLUE "%s" ANSI_COLOR_RESET "> ", directory);
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
                    printf("%s\n", command[1]);
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
            perror("error");
            return 1;
        }

        free(command);
        free(line);

    } while (exit != 0);

    return 0;
}