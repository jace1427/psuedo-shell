/* command.c
 *
 * Author: Justin Spidell
 *
 * Functions for each command of the Pseudo Shell.
 * These function only use system calls.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

/*	This function lists the contents of a directory
	input parameters:

	return parameters:

 */
void listDir()
{
	// Initialize pathname
	char pathname[4096];
	
	// Grab pathname
	getcwd(pathname, sizeof(pathname));

	// Open dir on pathname
	DIR* cwd = opendir(pathname);
	
	// Initialize dirent for file name
	struct dirent* file;

	// Loop and write each file / dir name
	while ((file = readdir(cwd)) != NULL) {
		write(1, file->d_name, strlen(file->d_name));
		write(1, " ", 1);
	}
	
	write(1, "\n", 1);
	
	// Close dir
	closedir(cwd);
}

/*	This funtion returns the current working directory
	input parameters:

	return parameters: 

 */
void showCurrentDir()
{
	// Initialize the pathname
	char pathname[PATH_MAX];
	
	// Grab pathname
	getcwd(pathname, sizeof(pathname));
	
	// write pathname
	write(1, pathname, strlen(pathname));
	write(1, "\n", 1);
}

/*	This function creates a new directory
	input parameters:
		char *dirName : a string representing the name of the
						new directory
	return parameters:

 */
void makeDir(char *dirName)
{
	// Make the directory
	int ret = mkdir(dirName, 00700);

	// Check for errors
	if (ret == -1) {
		write(1, "Error! Directory already exists\n", 33);
	}
}

/*	This function changes the current working directory
	input parameters:
		char *dirName : a string representing the directory to change to
	return parameters:

 */
void changeDir(char *dirName)
{
	// Change directories
	int ret = chdir(dirName);
	
	// Check for Errors
	if (ret == -1) {
		write(1, "Error! Directory does not exist\n", 33);
	}
}

/*	This function copies a file
	input parameters:
		char *sourcePath : a string representing the source path
		char *destinationPath : a string representing the destination path
	return parameters:

 */
void copyFile(char *sourcePath, char *destinationPath)
{
	// Open source file
	int sd = open(sourcePath, O_RDONLY);
	
	// Initialize stat for error checking on source file
	struct stat sb;
	stat(sourcePath, &sb);

	// Check if source file is a directory
	if (S_ISDIR(sb.st_mode)) {
		write(1, "Error! File is a directory\n", 28);
		return;

	// Check if file exists
	} else if (sd == -1) {	
		write(1, "Error! File does not exist\n", 28);
		return;
	}

	// Initialize stat for error checking on destination file
	stat(destinationPath, &sb);

	// If destination path is a directory, we need to add the source file
	// name to the path
	if (S_ISDIR(sb.st_mode)) {
		strcat(destinationPath, "/");
		strcat(destinationPath, sourcePath);
	}

	// Open destination path
	int dd = open(destinationPath, O_CREAT|O_WRONLY|O_TRUNC, 00700);

	// Initialize line buffer
	size_t len = 128;
	char* line_buf = malloc(len);

	// Initialize amount read for write()
	int amount_read;

	// Loop through the source file and read it 
	while ((amount_read = read(sd, line_buf, len)) != 0) {
		// Write to destination file
		write(dd, line_buf, amount_read);
	}
	
	// Free and close
	free(line_buf);
	close(sd);
	close(dd);
}

/*	This function moves a file from a source to a destination
	Similar to copy, but it deletes the source file
	input parameters:
		char *sourcePath : a string representing the source path
		char *destinationPath : a string representing the destination path
	return parameters:

*/
void moveFile(char *sourcePath, char *destinationPath)
{
	// open source path
	int sd = open(sourcePath, O_RDONLY);
	
	// Initialize stat for error checking on source file
	struct stat sb;
	stat(sourcePath, &sb);

	// Check if file is a directory
	if (S_ISDIR(sb.st_mode)) {
		write(1, "Error! File is a directory\n", 28);
		close(sd);
		return;

	// Check if file exists
	} else if (sd == -1) {	
		write(1, "Error! File does not exist\n", 28);
		close(sd);
		return;
	}

	// Initialize stat for error checking on destination file
	stat(destinationPath, &sb);

	// If destination path is a directory, we need to add the source file
	// name to the path
	if (S_ISDIR(sb.st_mode)) {
		strcat(destinationPath, "/");
		strcat(destinationPath, sourcePath);
	}

	// Open destination path	
	int dd = open(destinationPath, O_CREAT|O_WRONLY|O_TRUNC, 00700);

	// Initialize line buffer
	size_t len = 128;
	char* line_buf = malloc(len);

	// Initialize amount read for write()
	int amount_read;

	// Loop through the source file and read it
	while ((amount_read = read(sd, line_buf, len)) != 0) {
		// Write to destination file
		write(dd, line_buf, amount_read);
	}
	
	// Delete source file
	unlink(sourcePath);	

	// Free and close
	free(line_buf);
	close(sd);
	close(dd);
}

/*	This function deletes a file
	input parameters:
		char *filename: a string representing the file to be deleted
	return parameters:

 */
void deleteFile(char *filename)
{
	// Delete file
	int ret = unlink(filename);

	// Check for errors
	if (ret == -1) {
		write(1, "Error! File does not exist\n", 28);
	}
}

/*	This function displays the contents of a file
	input parameters:
		char *filename: a string representing the file to be displayed
	return parameters:

 */
void displayFile(char *filename)
{
	// Initialize and open the file
	int fd;
	fd = open(filename, O_RDONLY);
	
	// Initialize stat for error checking
	struct stat sb;
	stat(filename, &sb);

	// Check if file is a directory
	if (S_ISDIR(sb.st_mode)) {
		write(1, "Error! File is a directory\n", 28);
		return;
	}
	
	// Check if file exists
	if (fd == -1) {
		write(1, "Error! File does not exist\n", 28);
		return;
	}
	
	// Initialize line buffer
	size_t len = 128;
	char* line_buf = malloc(len);	
	
	// Initialize amount read for write()
	int amount_read;
	
	// Loop through the file and read it
	while ((amount_read = read(fd, line_buf, len)) != 0) {
		// write to stdout
		write(1, line_buf, amount_read);
	}
	
	// Free and close
	free(line_buf);
	close(fd);
}

