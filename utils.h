#define UTILS_H

char *readLine();

char **tokenize(char *command);

int isEmpty(char *string);

void freeMemory(char **command);

void printEcho(char **command, char *line, int child);

char *selectColor();

int presentPipe();

int presentRedirection1(char **arr);

int presentRedirection2(char **arr);

void processPipe(int index, char **command);

void processRedirection1(int index, char **command, char *line);

void processRedirection2(int index, char **command, char *line);
