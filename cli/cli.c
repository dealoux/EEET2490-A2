#include "cli.h"
#include "../printf/printf.h"
#include "../uart/uart1.h"
#include "../src/mbox.h"
#include "../src/string.h"

extern volatile unsigned int mBuf[];

// instantiate a list of commands
Command commandList[] = {
    {"help", "The help command serves as a user guide within the MyBareOS CLI environment. By simply typing help, users receive a concise list of available commands to aid navigation. For an in-depth understanding of a specific command, help followed by the desired command's name, like help showinfo, provides comprehensive details about its function and usage.", displayAllCommands},
    {"showinfo", "Show board revision and board MAC address.", displayBoardInfo},
    {"clear", "Clear the screen by decluttering the terminal's display. When exceuted, the screen will be scrolled down and the cursor will at the last bottom to give user a fresh screen.", clearScreen},
    {"setcolor", "the setcolor command allow user to adjust color of text and background in the terminal. The tag option '-t' is followed by the color of the text and the tag option '-b' is followed by the color of the background. For instance, setcolor -b yellow -t white will create a yellow background with white text. Avaiable colors: black, red, green, yellow, blue, purple, cyan, white.", setConsoleColor},
};

// Instantiate the colors
const ColorMap colorMappings[] = {
    {"black", "\033[1;30m", "\x1b[40m"},
    {"red", "\033[1;31m", "\x1b[41m"},
    {"green", "\033[1;32m", "\x1b[42m"},
    {"yellow", "\033[1;33m", "\x1b[43m"},
    {"blue", "\033[1;34m", "\x1b[44m"},
    {"purple", "\033[1;35m", "\x1b[45m"},
    {"cyan", "\033[1;36m", "\x1b[46m"},
    {"white", "\033[1;37m", "\x1b[47m"}};

const char *findTextColor(const char *colorStr)
{
    for (size_t i = 0; i < sizeof(colorMappings) / sizeof(ColorMap); i++)
    {
        if (strcmp(colorStr, colorMappings[i].colorName) == 0)
        {
            return colorMappings[i].textColorAscii;
        }
    }
    return NULL; // Not found
}

const char *findAsciiBgColor(const char *colorStr)
{
    for (size_t i = 0; i < sizeof(colorMappings) / sizeof(ColorMap); i++)
    {
        if (strcmp(colorStr, colorMappings[i].colorName) == 0)
        {
            return colorMappings[i].bgColorAscii;
        }
    }
    return NULL; // Not found
}

void processCommand(char *command)
{
    // seperate the commands and arguements
    char *commandName = strtok(command, " ");
    char *args = strtok(NULL, "");
    for (size_t i = 0; i < sizeof(commandList) / sizeof(Command); i++)
    {
        if (strcmp(commandName, commandList[i].name) == 0)
        {
            commandList[i].handler(args);
            return;
        }
    }
    printf("\nCommand not found: %s. Please use help to view all valid commands.", commandName);
}

void displayAllCommands(char *args)
{
    if (args && *args) // check if the arg and its value is not null
    {
        for (size_t i = 0; i < sizeof(commandList) / sizeof(Command); i++)
        {
            if (strcmp(args, commandList[i].name) == 0)
            {
                printf("\n--%s: \n%s\n", commandList[i].name, commandList[i].description);
                return; // Exit after displaying the matched command
            }
        }
        printf("\nCommand '%s' not found.\n", args); // If no command matched
    }
    else // Display all commands if no specific command name is provided
    {
        printf("\nAvailable commands:\n");
        for (size_t i = 0; i < sizeof(commandList) / sizeof(Command); i++)
        {
            printf("--%s: \n%s\n", commandList[i].name, commandList[i].description);
        }
    }
}

void clearScreen(char *args)
{
    printf("\033[2J\033[1;1H");
}

void setConsoleColor(char *args)
{
    char *token = strtok(args, " ");
    while (token)
    {
        const char *asciiColor = NULL;
        printf("\n%s", token);
        if (strcmp(token, "-t") == 0)
        {
            token = strtok(NULL, " ");

            if (token)
            {
                asciiColor = findTextColor(token); // retrieve the color input
                if (asciiColor)
                {
                    printf(asciiColor); // Set text color
                    printf("\n");
                }
                else
                {
                    printf("Invalid color: %s, please type help setcolor to find valid colors\n", token);
                }
            }
        }
        else if (strcmp(token, "-b") == 0)
        {
            token = strtok(NULL, " ");
            if (token)
            {
                asciiColor = findAsciiBgColor(token);
                if (asciiColor)
                {
                    printf(asciiColor); // Set background color
                    printf("\n");
                    return;
                }
                else
                {
                    printf("Invalid color: %s, please type help setcolor to find valid colors\n", token);
                }
            }
        }

        token = strtok(NULL, " ");
    }
}

void autocompleteHandler(char *buffer, int *index)
{
    // For simplicity, autocomplete with the first matching command
    for (int i = 0; i < sizeof(commandList) / sizeof(Command); i++)
    {
        if (strstr(commandList[i].name, buffer) == commandList[i].name)
        {
            // found a match, complete the buffer
            strcpy(buffer, commandList[i].name);
            *index = strlen(buffer);
            printf("\rMyBareOS> %s", buffer); // refresh the CLI prompt
            return;
        }
    }
}

void displayBoardInfo(char *args)
{
    unsigned int *response = 0;

    printf("after setup\n");
    // display board serial
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETMODEL, &response, 4, 0);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("\nBoard model %16c %d\n", ':', response[0]);

    // display board serial
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETSERIAL, &response, 8, 0);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("Board serial %15c %d\n", ':', response[0]);

    // display mac address
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_MACADDR, &response, 6, 0);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    unsigned char bytes[6];
    bytes[0] = (response[0] >> 24) & 0xFF; // extract the most significant byte (1st byte)
    bytes[1] = (response[0] >> 16) & 0xFF; // 2nd byte
    bytes[2] = (response[0] >> 8) & 0xFF;  // 3rd byte
    bytes[3] = (response[0]) & 0xFF;       // 4th byte
    bytes[4] = (response[1] >> 8) & 0xFF;  // 5th byte (1st byte of the 2nd half of the MAC address)
    bytes[5] = (response[1]) & 0xFF;       // 6th byte (2nd byte of the 2nd half of the MAC address)
    printf("Board MAC address %10c %x:%x:%x:%x:%x:%x\n", ':', bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);

    // display board revision
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETBOARDREVISION, &response, 4, 0);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("Board revision %13c %x\n", ':', response[0]);

    // display ARM memory
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_ARM_MEMORY, &response, 4, 0);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("ARM memory %17c %dMB\n", ':', response[0] / 10485760); // convert to megabytes
    // display VC memory
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_VC_MEMORY, &response, 8, 0);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("VC memory %18c %dMB\n", ':', response[0] / 10485760); // convert to megabytes

    // display clock rate of arm
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETCLKRATE, &response, 8, 0, 3);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("ARM clock rate %13c %dMHz\n", ':', response[0] / 1000000); // convert to MH

    // display clock rate of uart
    mbox_buffer_setup(ADDR(mBuf), MBOX_TAG_GETCLKRATE, &response, 8, 0, 2);
    mbox_call(ADDR(mBuf), MBOX_CH_PROP);
    printf("UART clock rate %12c %dMHz\n", ':', response[0] / 1000000); // convert to MH
}
