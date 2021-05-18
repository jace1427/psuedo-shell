/* main.c
 *
 * Author: Justin Spidell
 *
 * Program driver for Pseudo Shell.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "command.h"
#include "string_parser.h"

/*  This function calls the corrisponding function in command.c
	input parameters:
		char** command_list : an array of strings grabbed from main
		int num_tokens : the length of the array

	return parameters:
		int : 1 if the command is exit, otherwise 0
 */
int command(char** command_list, int num_tokens)
{
	// Check for incorrect syntax, i.e. 'ls ls'
	for (int i = 0; command_list[i + 1] != NULL; i++) {
		if (!strcmp(command_list[i], command_list[i + 1])) {
			write(1, "Error! Incorrect syntax. No control code found.\n", 49);
			return 0;
		}
	}

	// Find the correct command, check the # of parameters
	if (!(strcmp(command_list[0], "ls"))) {
		if (num_tokens == 1) {
			listDir();
		} else {
			goto PARAMETER_ERROR;
		}
		
	} else if (!(strcmp(command_list[0], "pwd"))) {
		if (num_tokens == 1) {
			showCurrentDir();
		} else {
			goto PARAMETER_ERROR;
		}

	} else if (!(strcmp(command_list[0], "mkdir"))) {
		if (num_tokens == 2) {
			makeDir(command_list[1]);
		} else {
			goto PARAMETER_ERROR;
		}

	} else if (!(strcmp(command_list[0], "cd"))) {
		if (num_tokens == 2) {
			changeDir(command_list[1]);
		} else {
			goto PARAMETER_ERROR;
		}

	} else if (!(strcmp(command_list[0], "cp"))) {
		if (num_tokens == 3) {
			copyFile(command_list[1], command_list[2]);
		} else {
			goto PARAMETER_ERROR;
		}

	} else if (!(strcmp(command_list[0], "mv"))) {
		if (num_tokens == 3) {
			moveFile(command_list[1], command_list[2]);
		} else {
			goto PARAMETER_ERROR;
		}

	} else if (!(strcmp(command_list[0], "rm"))) {
		if (num_tokens == 2) {
			deleteFile(command_list[1]);
		} else {
			goto PARAMETER_ERROR;
		}

	} else if (!(strcmp(command_list[0], "cat"))) {
		if (num_tokens == 2) {
			displayFile(command_list[1]);
		} else {
			goto PARAMETER_ERROR;
		}
	
	} else if (!(strcmp(command_list[0], "exit"))) {
		return 1;
	
	// Unrecognized command
	} else {
		write(1, "Error! Unrecognized command: ", 29);
		write(1, command_list[0], strlen(command_list[0]));
		write(1, "\n\n", 4);
	}
	
	return 0;	

	// Don't like using goto's, but couldn't find a better way
	PARAMETER_ERROR:
		write(1, "Error! Unsupported parameters for command: ", 43);
		write(1, command_list[0], strlen(command_list[0]));
		write(1, "\n", 2);
	
	return 0;
}

/*	Psuedo Shell Program Driver
	Grabs input from either the interactive shell or a file and calls 
	the command function.
	input parameters:
		int argc : the number of command line arguments
		char const *argv[] : an array of strings containing the command line arguments
	returns:
		int : Always 0
 */
int main(int argc, char const *argv[])
{	
	// bool interactive so we know which mode we are in later
	int interactive;
	
	// inFPtr will either be stdin or grabbed from argv
	FILE *inFPtr;
	
	// Check for interactive vs file mode
	if (argc < 2) {
		interactive = 1;
		write(1, ">>> ", 4);
		inFPtr = stdin;
	
	} else {
		interactive = 0;
		
		// Check flags
		if (!(strcmp(argv[1], "-f"))) {
			inFPtr = fopen(argv[2], "r");		
			
			// Check file exists
			if (inFPtr == NULL) {
				write(1, "Error! file does not exist\n", 28);
				fclose(inFPtr);
				return 0;
			}

			// Redirect stdout
			freopen("output.txt", "w", stdout); 
		
		// Unknown flags
		} else {
			write(1, "Error! Unknown Flag\n", 21);
			return 0;
		}
	}

	// Initialize line_buf
	size_t len = 128;
	char* line_buf = malloc(len);

	// Initialize small and large token buffers
	command_line small_token_buffer;
	command_line large_token_buffer;

	// Main While loop
	while (getline(&line_buf, &len, inFPtr) != -1) {
		
		// break if 'exit'
		if (!(strcmp(line_buf, "exit\n"))) {
			break;
		
		// If we get a newline, prompt again and restart the loop
		} else if (!(strcmp(line_buf, "\n"))) {
			if (interactive)
				write(1, ">>> ", 4);
			continue;
		}

		// Fill large buffer
		large_token_buffer = str_filler(line_buf, ";");

		// Loop through large buffer
		for (int i = 0; large_token_buffer.command_list[i] != NULL; i++) {
			
			// Fill small buffer
			small_token_buffer = str_filler(large_token_buffer.command_list[i], " ");

			// Call command with small buffer
			int exit = command(small_token_buffer.command_list, small_token_buffer.num_token);

			// free small buffer 
			free_command_line(&small_token_buffer);
			memset(&small_token_buffer, 0, 0);
			
			// if the command was exit, free and return
			if (exit) {
				free(line_buf);
				free_command_line(&large_token_buffer);
				fclose(inFPtr);
				return 0;
			}
		}
		
		// free large buffer
		free_command_line(&large_token_buffer);
		memset(&large_token_buffer, 0, 0);
		
		// Prompt for next command
		if (interactive)
			write(1, ">>> ", 4);
	}

	// Free and return
	free(line_buf);
	fclose(inFPtr);
	return 0;
}

