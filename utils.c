#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define ANSI_COLOR_GREEN "\e[0;32m"
#define ANSI_COLOR_BLUE "\e[0;34m"
#define ANSI_COLOR_RED "\e[0;31m"
#define ANSI_COLOR_YELLOW "\e[0;33m"
#define ANSI_COLOR_PURPLE "\e[0;35m"
#define ANSI_COLOR_CYAN "\e[0;36m"
#define ANSI_COLOR_WHITE "\e[0;37m"

#include "utils.h"

#define BUFFER_SIZE 64

char *readLine()
{
  char *buffer = malloc(BUFFER_SIZE * sizeof(char));

  if (buffer == NULL)
  {
    fprintf(stderr, "Memory allocation failed 1\n");
    exit(EXIT_FAILURE);
  }

  int index = 0;
  int bufferSize = BUFFER_SIZE;
  char c;

  while ((c = getchar()) != '\n')
  {
    if (index == bufferSize - 1)
    {
      bufferSize *= 2;
      buffer = realloc(buffer, bufferSize);
      if (buffer == NULL)
      {
        fprintf(stderr, "Memory allocation failed 2\n");
        exit(EXIT_FAILURE);
      }
    }

    buffer[index++] = c;
  }

  buffer[index] = '\0';

  buffer = realloc(buffer, (index + 1) * sizeof(char));

  if (buffer == NULL)
  {
    fprintf(stderr, "Memory reallocation failed\n");
    exit(EXIT_FAILURE);
  }

  return buffer;
}

char **tokenize(char *command)
{
  char **args;
  int argCounter = 0;
  char *aux = command;
  char *copy;

  while (*aux != '\0')
  {
    if (*aux == ' ')
      argCounter++;
    aux++;
  }

  args = malloc((argCounter + 1) * sizeof(char *));

  aux = command;
  argCounter = 0;
  copy = strtok(aux, " ");
  while (copy != NULL)
  {
    args[argCounter] = strdup(copy);
    argCounter++;
    copy = strtok(NULL, " ");
  }
  args[argCounter] = NULL;

  return args;
}

int isEmpty(char *string)
{
  if (string == NULL)
    return 1;

  while (*string != '\0')
  {
    if (*string != ' ')
      return 0;
    string++;
  }

  return 1;
}

char *selectColor(char *color)
{
  if (strncmp(color, "red", 4) == 0)
  {
    return ANSI_COLOR_RED;
  }

  if (strncmp(color, "green", 6) == 0)
  {
    return ANSI_COLOR_GREEN;
  }

  if (strncmp(color, "blue", 5) == 0)
  {
    return ANSI_COLOR_BLUE;
  }

  if (strncmp(color, "white", 6) == 0)
  {
    return ANSI_COLOR_WHITE;
  }

  if (strncmp(color, "yellow", 7) == 0)
  {
    return ANSI_COLOR_YELLOW;
  }

  if (strncmp(color, "purple", 7) == 0)
  {
    return ANSI_COLOR_PURPLE;
  }

  if (strncmp(color, "cyan", 5) == 0)
  {
    return ANSI_COLOR_CYAN;
  }

  return NULL;
}

int presentPipe(char **arr)
{
  int index = 0;
  while (arr[index] != NULL)
  {
    if (strncmp(arr[index], "|", 2) == 0)
    {
      return index;
    }
    index++;
  }
  return -1;
}

int presentRedirection1(char **arr)
{
  int index = 0;
  while (arr[index] != NULL)
  {
    if (strncmp(arr[index], ">", 2) == 0)
    {
      return index;
    }
    index++;
  }
  return -1;
}

int presentRedirection2(char **arr)
{
  int index = 0;
  while (arr[index] != NULL)
  {
    if (strncmp(arr[index], ">>", 3) == 0)
    {
      return index;
    }
    index++;
  }
  return -1;
}

void processPipe(int index, char **command)
{
  int auxIndex0 = 0, auxIndex1 = 0, auxIndex2 = 0, commandLength = 0;
  int pipe_fds[2];
  char **aux = command;
  char **command1, **command2;

  while (command[commandLength] != NULL)
  {
    commandLength++;
  }

  command1 = malloc((index + 1) * sizeof(char *));
  command2 = malloc((commandLength - index) * sizeof(char *));

  while (aux[auxIndex0] != NULL)
  {
    if (auxIndex0 < index)
    {
      command1[auxIndex1] = strdup(aux[auxIndex0]);
      auxIndex1++;
    }
    else if (auxIndex0 > index)
    {
      command2[auxIndex2] = strdup(aux[auxIndex0]);
      auxIndex2++;
    }
    auxIndex0++;
  }

  command1[auxIndex1] = NULL;
  command2[auxIndex2] = NULL;

  if (pipe(pipe_fds) == -1)
  {
    perror("pipe failed");
  }

  pid_t child1 = fork();

  if (child1 == 0)
  {
    dup2(pipe_fds[0], STDIN_FILENO);
    close(pipe_fds[1]);
    execvp(command2[0], command2);
  }
  else if (child1 < 0)
  {
    perror("fork failed");
  }

  pid_t child2 = fork();

  if (child2 == 0)
  {
    dup2(pipe_fds[1], STDOUT_FILENO);
    close(pipe_fds[0]);
    execvp(command1[0], command1);
  }
  else if (child2 < 0)
  {
    perror("fork failed");
  }

  waitpid(child1, NULL, 0);
  waitpid(child2, NULL, 0);
  close(pipe_fds[1]);
  close(pipe_fds[0]);
  free(command1);
  free(command2);
}

void processRedirection1(int index, char **command)
{
  char *path;
  int fd;
  char **command1 = malloc((index + 1) * sizeof(char *));

  if (isEmpty(command[index + 1]) == 0)
    path = command[index + 1];

  for (int i = 0; i < index; i++)
  {
    command1[i] = strdup(command[i]);
  }
  command1[index] = NULL;

  pid_t pid = fork();

  if (pid == 0)
  {
    if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) != -1)
    {
      dup2(fd, 1);
      close(fd);
      execvp(command1[0], command1);
      exit(EXIT_FAILURE);
    }
  }
  else if (pid < 0)
  {
    perror("fork failed");
  }
  else
  {
    wait(NULL);
  }
}

void processRedirection2(int index, char **command)
{
  char *path;
  int fd;
  char **command1 = malloc((index + 1) * sizeof(char *));

  if (isEmpty(command[index + 1]) == 0)
    path = command[index + 1];

  for (int i = 0; i < index; i++)
  {
    command1[i] = strdup(command[i]);
  }
  command1[index] = NULL;

  pid_t pid = fork();

  if (pid == 0)
  {
    if ((fd = open(path, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) != -1)
    {
      dup2(fd, 1);
      close(fd);
      execvp(command1[0], command1);
      exit(EXIT_FAILURE);
    }
  }
  else if (pid < 0)
  {
    perror("fork failed");
  }
  else
  {
    wait(NULL);
  }
}
