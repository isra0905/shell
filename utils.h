#define UTILS_H

char *readLine();

char **tokenize(char *command);

int isEmpty(char *string);

char *selectColor();

int presentPipe();

void processPipe(int index, char **command);