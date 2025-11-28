#include<stdio.h>
#include<string.h>

enum filemonitor_error_code {
	FILEMONITOR_SUCCESS = 0,
	FILEMONITOR_CLEAR = 0,
	FILEMONITOR_FAILURE,
	FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS,
	FILEMONITOR_FAILURE_UNKNOWN_OPTION
};

enum filemonitor_error_code filemonitor_exec(void);
enum filemonitor_error_code filemonitor_check_config(void);
enum filemonitor_error_code filemonitor_edit_config(void);
void filemonitor_print_help(void);
void filemonitor_print_error_codes(void);

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
	} else if (strcmp(option, "--edit-config") == 0) {
		result = filemonitor_edit_config();
	} else if (strcmp(option, "--error-codes") == 0) {
		filemonitor_print_error_codes();
	} else if (strcmp(option, "--help") == 0) {
		filemonitor_print_help();
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
	return FILEMONITOR_SUCCESS;
}

void filemonitor_print_help(void)
{
	printf("FileMonitor: Help Page\n");
	printf("Options:\n");
	printf("\t--check-config\t\tChecks the config file to\n");
	printf("\t\t\t\tMake sure the formatting is correct.\n");
	printf("\t\t\t\tNB. It will remove any bad formatting\n");
	printf("\t--edit-config\t\tModifies the config file\n");
	printf("\t--help\t\t\tBrings up this help page\n");
	printf("\t--run\t\t\tRuns FileMonitor\n");
}

void filemonitor_print_error_codes(void)
{
	printf("FileMonitor: Error Codes Page\n");
	printf("\t%d\t= SUCCESS\n", FILEMONITOR_SUCCESS);
	printf("\t%d\t= FileMonitor encountered a general failure.\n", FILEMONITOR_FAILURE);
	printf("\t%d\t= FileMonitor received the wrong number of arguments.\n", FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS);
	printf("\t%d\t= FileMonitor received an unknown option.\n", FILEMONITOR_FAILURE_UNKNOWN_OPTION);
}
