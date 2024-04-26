#include "cli.h"
#include "command.h"
#include "printf.h"
#include "../uart/uart1.h"
#include "../kernel/mbox.h"
#include "../kernel/string.h"

extern volatile unsigned int mBuf[];

void processCommand(char *command){
  // seperate the commands and arguements
  char *commandName = strtok(command, " ");
  char *args = strtok(NULL, "");
  for (size_t i = 0; i < sizeof(commandList) / sizeof(Command); i++)
  {
    if (strcmp(commandName, commandList[i].name) == 0){
      commandList[i].handler(args);
      return;
    }
  }
  printf("\nCommand not found: %s. Please use help to view all valid commands.", commandName);
}

void autocompleteHandler(char *buffer, int *index)
{
  // For simplicity, autocomplete with the first matching command
  for (int i = 0; i < sizeof(commandList) / sizeof(Command); i++){
    if (strstr(commandList[i].name, buffer) == commandList[i].name){
      // found a match, complete the buffer
      strcpy(buffer, commandList[i].name);
      *index = strlen(buffer);
      printf("\rMyBareOS> %s", buffer); // refresh the CLI prompt
      return;
    }
  }
}

void initCli(){
  printf("\nWelcome to MyBareOS CLI. Type 'help' for a list of commands.\n");
}