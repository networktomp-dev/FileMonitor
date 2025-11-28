#include<stdio.h> /* perror */
#include<stdlib.h> /* fopen() */
#include<string.h> /* strcmp()*/
#include<unistd.h> /* fork() */
#include<sys/types.h> /* pid_t */
#include<sys/wait.h> /* waitpid() */


enum filemonitor_error_code {
	FILEMONITOR_SUCCESS = 0,
	FILEMONITOR_CLEAR = 0,
	FILEMONITOR_FAILURE,
	FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS,
	FILEMONITOR_FAILURE_UNKNOWN_OPTION,
	FILEMONITOR_FAILURE_EDIT_CONFIG_WAITPID_FAILED,
	FILEMONITOR_FAILURE_EDIT_CONFIG_FORK_FAILED,
	FILEMONITOR_FAILURE_EDIT_CONFIG_VIM_NOT_INSTALLED
};

enum filemonitor_error_code filemonitor_exec(void);
enum filemonitor_error_code filemonitor_check_config(void);
enum filemonitor_error_code filemonitor_edit_config(void);
enum filemonitor_error_code filemonitor_check_log(void);
void filemonitor_print_help(void);
void filemonitor_print_error_codes(void);
void filemonitor_print_version(void);

int main (int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "FileMonitor expects a max of one option only\n");
		return FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS;
	}

	enum filemonitor_error_code result = FILEMONITOR_CLEAR;
	char *option = argv[1];
	if (strcmp(option, "--run") == 0) {
		result = filemonitor_exec();
		if (result != FILEMONITOR_SUCCESS) {
			goto cleanup;
		}
	} else if (strcmp(option, "--check-config") == 0) {
		result = filemonitor_check_config();
		if (result != FILEMONITOR_SUCCESS) {
			goto cleanup;
		}
	} else if (strcmp(option, "--check-log") == 0) {
		result = filemonitor_check_log();
		if (result != FILEMONITOR_SUCCESS) {
			goto cleanup;
		}
	} else if (strcmp(option, "--edit-config") == 0) {
		if (result != FILEMONITOR_SUCCESS) {
			goto cleanup;
		}
		result = filemonitor_edit_config();
	} else if (strcmp(option, "--error-codes") == 0) {
		filemonitor_print_error_codes();
	} else if (strcmp(option, "--help") == 0) {
		filemonitor_print_help();
	} else if (strcmp(option, "--version") == 0) {
		filemonitor_print_version();
	} else {
		fprintf(stderr, "FileMonitor: Error: Option not recognised\n");
		fprintf(stderr, "For more info, type:\n");
		fprintf(stderr, "\tFileMonitor --help\n");
		return FILEMONITOR_FAILURE_UNKNOWN_OPTION;
	}

	return FILEMONITOR_SUCCESS;

cleanup:
	fprintf(stderr, "FileMonitor: Error code %d\n", result);
	return FILEMONITOR_FAILURE;
}

enum filemonitor_error_code filemonitor_exec(void)
{
	return FILEMONITOR_SUCCESS;
}

enum filemonitor_error_code filemonitor_check_config(void)
{
	return FILEMONITOR_SUCCESS;
}

enum filemonitor_error_code filemonitor_edit_config(void)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		return FILEMONITOR_FAILURE_EDIT_CONFIG_FORK_FAILED;
	} else if (pid == 0) {
		execlp("vim", "vim", "/home/networktom/C/src/FileMonitor/FileMonitor.conf", (char *)NULL);
		perror("execlp failed to launch vim");
    		exit(FILEMONITOR_FAILURE_EDIT_CONFIG_VIM_NOT_INSTALLED);
	} else {
		pid_t terminated_pid = waitpid(pid, NULL, 0);
		if (terminated_pid == -1) {
			return FILEMONITOR_FAILURE_EDIT_CONFIG_WAITPID_FAILED;
		}
	}

	printf("Finished editing FileMonitor.conf\n");
	printf("Please test it first with:\n");
	printf("\tFileMonitor --check-config\n");

	return FILEMONITOR_SUCCESS;
}

enum filemonitor_error_code filemonitor_check_log(void)
{
	return FILEMONITOR_SUCCESS;
}

void filemonitor_print_help(void)
{
	printf("FileMonitor: Help Page\n");
	printf("Options:\n");
	printf("\t--check-config\tChecks the config file to\n");
	printf("\t\t\tMake sure the formatting is correct.\n");
	printf("\t\t\tNB. It will remove any bad formatting\n");
	printf("\t--check-log\tEnters a menu to choose a\n");
	printf("\t\t\tlog file then prints the choice to\n");
	printf("\t\t\tscreen\n");
	printf("\t--edit-config\tModifies the config file\n");
	printf("\t--help\t\tBrings up this help page\n");
	printf("\t--run\t\tRuns FileMonitor\n");
	printf("\t--version\tPrints the current version of FileMonitor\n");
}

void filemonitor_print_error_codes(void)
{
	printf("FileMonitor: Error Codes Page\n");
	printf("\t%d\t= SUCCESS\n", FILEMONITOR_SUCCESS);
	printf("\t%d\t= Encountered a general failure.\n", FILEMONITOR_FAILURE);
	printf("\t%d\t= Received the wrong number of arguments.\n", FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS);
	printf("\t%d\t= Received an unknown option.\n", FILEMONITOR_FAILURE_UNKNOWN_OPTION);
	printf("\t%d\t= Could not fork a new process when attempting to edit FileMonitor.conf in function filemonitor_edit_config\n", FILEMONITOR_FAILURE_EDIT_CONFIG_FORK_FAILED);
	printf("\t%d\t= waitpid() failed in function filemonitor_edit_config()\n", FILEMONITOR_FAILURE_EDIT_CONFIG_WAITPID_FAILED);
	printf("\t%d\t= Could not find Vim in function filemonitor_edit_config()\n", FILEMONITOR_FAILURE_EDIT_CONFIG_VIM_NOT_INSTALLED);
}

void filemonitor_print_version(void)
{
	printf("FileMonitor Version = 1.0.0.0\n");
	printf("Download and install the latest version with:\n");
	printf("git clone https://github.com/networktomp-dev/FileMonitor.git\n");
	printf("cd FileMonitor\n");
	printf("make\n");
}
