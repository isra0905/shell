#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define BUFFER_SIZE 64

char *readLine()
{
  char *buffer = malloc(BUFFER_SIZE * sizeof(char));

  if (buffer == NULL)
  {
    fprintf(stderr, "Memory allocation failed\n");
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
        fprintf(stderr, "Memory allocation failed\n");
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
