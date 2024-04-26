#include "../uart/uart1.h"
#include "../printf/printf.h"
#include "../cli/cli.h"
#include "string.h"

#define MAX_CMD_SIZE 100
#define MAX_HISTORY 20

void displayWelcomeMessage();

char commandHistory[MAX_HISTORY][MAX_CMD_SIZE];
int historyIndex = -1;
int lastCommandIndex = 0;

void cli()
{
	static char cli_buffer[MAX_CMD_SIZE];
	static int index = 0;
	static int isNewCommand = 1;

	// print prompt
	if(isNewCommand){
		printf("\n");
		printf("MyBareOS> ");
		isNewCommand = 0;
	}

	// read and send back each char
	char c = uart_getc();
	
	// handle special 
	
	switch(c){
		// handle backspace & delete
		case 0x7F:
		case 0x08:
			if(index > 0) {
				uart_sendc(0x08);		  // move cursor backwards
				uart_sendc(' ');		  // overwrite the last character with space
				uart_sendc(0x08);		  // move cursor backwards again
				index--;				  // decrement buffer index
				cli_buffer[index] = '\0'; // "delete" last char in buffer
			}
			break;

		case '+':
			if(historyIndex > -1){
				historyIndex--;
				if (historyIndex == -1)	  // back to the newest command
					cli_buffer[0] = '\0'; // empty the buffer
				else
					strcpy(cli_buffer, commandHistory[historyIndex]);

				printf("\rMyBareOS> %s", cli_buffer);
				index = strlen(cli_buffer);
			}
			break;

		case '_':
			if (historyIndex < lastCommandIndex - 1){
				historyIndex++;
				strcpy(cli_buffer, commandHistory[historyIndex]);
				printf("\rMyBareOS> %s", cli_buffer);
				index = strlen(cli_buffer);
			}
			break;

		case '\t':
			autocompleteHandler(cli_buffer, &index);
			break;

		case '\n':
			cli_buffer[index] = '\0';

			// save command to history
			strcpy(commandHistory[lastCommandIndex % MAX_HISTORY], cli_buffer);
			lastCommandIndex++;
			if (lastCommandIndex >= MAX_HISTORY)
				lastCommandIndex = 0; // start overwriting oldest commands
			historyIndex = -1;		  // reset history position

			/* Compare with supported commands and execute
			 * ........................................... */
			processCommand(cli_buffer);
			isNewCommand = 1;
			index = 0;
			break;

		default:
			uart_sendc(c);		   // echo the character
			cli_buffer[index] = c; // Store into the buffer
			index++;
			break;
	}

	// prevent buffer overflow
	if (index >= MAX_CMD_SIZE){
		index = 0;
	}
}

void main(){
	// set up serial console
	uart_init();
	displayWelcomeMessage();

	// run CLI
	while (1){
		cli();
	}
}

void displayWelcomeMessage(){
	// ANSI escape code for red text
	const char *greenColor = "\033[1;32m";
	const char *yellowColor = "\033[1;33m";
	// ANSI escape code to reset to default
	const char *resetColor = "\033[0m";

	printf("%s", yellowColor); // Set the text color to yellow
	printf("oooooooooooo oooooooooooo oooooooooooo ooooooooooooo    .oooo.         .o    .ooooo.     .oooo.\n");
	printf("`888'     `8 `888'     `8 `888'     `8 8'   888   `8  .dP  888b      .d88   888' `Y88.  d8P'`Y8b\n");
	printf(" 888          888          888              888             ]8P'   .d'888   888    888 888    888\n");
	printf(" 888oooo8     888oooo8     888oooo8         888           .d8P'  .d'  888    'Vbood888 888    888\n");
	printf(" 888          888          888              888         .dP'     88ooo888oo       888' 888    888\n");
	printf(" 888       o  888       o  888       o      888       .oP     .o      888       .88P'  `88b  d88'\n");
	printf("o888ooooood8 o888ooooood8 o888ooooood8     o888o      8888888888     o888o    .oP'      `Y8bd8P'\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("oooooooooo.        .o.       ooooooooo.   oooooooooooo         .oooooo.    .oooooo..o\n");
	printf("`888'   `Y8b      .888.      `888   `Y88. `888'     `g        d8P'  `Y8b  d88P'   `Y8\n");
	printf(" 888     888     .8 888.      888   .d88'  888               888      888 Y88bo.\n");
	printf(" 888oooo888'    .8' `888.     888ooo88P'   888oooo8          888      888   `Y8888o.\n");
	printf(" 888    `88b   .88ooo8888.    888`88b.     888               888      888      ` Y88b\n");
	printf(" 888    .88P  .8'     `888.   888  `88b.   888       o       `88b    d88' oo     .d8P\n");
	printf("o888bood8P'  o88o     o8888o o888o  o888o o888ooooood8        `Y8bood8P'  888888P'\n");
	printf("\n");
	printf("%s", greenColor); // green footer
	printf("    Developed by Le Minh Duc - s4000577\n");

	printf("%s", resetColor); // default text color
}