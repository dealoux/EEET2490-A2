#ifndef CLI_H
#define CLI_H

// Function type for command handlers
typedef void (*CommandFunction)(char *args);

// Declarations
void processCommand(char *command);
void autocompleteHandler(char *buffer, int *index);
void initCli();

#endif