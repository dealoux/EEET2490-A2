#ifndef CLI_H
#define CLI_H

// Function type for command handlers
typedef void (*CommandFunction)(char *args);

// Struct to represent a command
typedef struct{
  const char *name;        // Command name
  const char *description; // Short description
  CommandFunction handler; // Function to handle the command
} Command;

typedef struct{
  const char *colorName;      // Color name
  const char *textColorAscii; // Font color
  const char *bgColorAscii;   // Background colors
} ColorMap;

// Public function to process a command
void processCommand(char *command);

// Declarations
void displayAllCommands(char *args);
void clearScreen(char *args);
void setConsoleColor(char *args);
void displayBoardInfo(char *args);
const char *findTextColor(const char *colorStr);
const char *findAsciiBgColor(const char *colorStr);
void autocompleteHandler(char *buffer, int *index);

#endif