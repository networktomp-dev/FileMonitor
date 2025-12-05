#include<stdio.h> /* perror */
#include<stdlib.h> /* fopen() */
#include<string.h> /* strcmp()*/
#include<unistd.h> /* fork() */
#include<sys/types.h> /* pid_t */
#include<sys/wait.h> /* waitpid() */
#include<stdbool.h> /* bool */
#include<errno.h> /* errno */
#include<stdint.h> /* uint64_t uint32_t uint8_t */

#include"pvars.h" /* plist_t */

#define LINE_MAX 256
#define CONFIG_PATH "/etc/FileMonitor.conf"
#define TEMP_CONFIG_PATH "/etc/FileMonitor.conf.tmp"
#define LOG_PATH "/var/log/FileMonitor/FileMonitor.log"
#define ERROR_LOG_PATH "/var/log/FileMonitor/FileMonitor_error.log"

enum filemonitor_error_code {
	FILEMONITOR_SUCCESS = 0,
	FILEMONITOR_CLEAR = 0,
	FILEMONITOR_FAILURE,
	FILEMONITOR_FAILURE_IS_NOT_ROOT,
	FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS,
	FILEMONITOR_FAILURE_UNKNOWN_OPTION,
	FILEMONITOR_FAILURE_EDIT_CONFIG_WAITPID_FAILED,
	FILEMONITOR_FAILURE_EDIT_CONFIG_FORK_FAILED,
	FILEMONITOR_FAILURE_EDIT_CONFIG_VIM_NOT_INSTALLED,
	FILEMONITOR_FAILURE_CHECK_CONFIG_FOPEN_FAILED,
	FILEMONITOR_FAILURE_CHECK_CONFIG_FPUTS_FAILED,
	FILEMONITOR_FAILURE_CHECK_CONFIG_RENAME_FAILED,
	FILEMONITOR_FAILURE_CHECK_LOG_FORK_FAILED,
	FILEMONITOR_FAILURE_CHECK_LOG_VIM_NOT_INSTALLED,
	FILEMONITOR_FAILURE_CHECK_LOG_WAITPID_FAILED,
	FILEMONITOR_RUN_GET_CONFIG_FAILED,
	FILEMONITOR_FAILURE_VIEW_ERROR_LOG_FORK_FAILED,
	FILEMONITOR_FAILURE_VIEW_ERROR_LOG_LESS_NOT_INSTALLED,
	FILEMONITOR_FAILURE_VIEW_ERROR_LOG_WAITPID_FAILED
};

struct config_data {
	char *admin;
	char *adminemail;
	uint64_t file_max_size;
	uint64_t log_max_size;
	uint32_t dir_max_size;
	uint8_t log_retention;
	plist_t *ignore_dir;
	plist_t *ignore_file;
	plist_t *monitor_file;
};

bool is_running_as_root(void);
enum filemonitor_error_code filemonitor_run(void);
struct config_data *filemonitor_get_config(void);
enum filemonitor_error_code filemonitor_check_config(void);
void filemonitor_print_to_error_log(enum filemonitor_error_code result);
bool is_valid_config_line(const char *line);
enum filemonitor_error_code filemonitor_check_log(void);
enum filemonitor_error_code filemonitor_edit_config(void);
void filemonitor_print_help(void);
void filemonitor_print_error_codes(void);
enum filemonitor_error_code filemonitor_view_error_log(void);
void filemonitor_print_version(void);

int main (int argc, char *argv[])
{
	if (is_running_as_root() == false) {
		fprintf(stderr, "ERROR: FileMonitor requires root privileges to run\n");
		fprintf(stderr, "Run with sudo:\n");
		fprintf(stderr, "\tsudo FileMonitor [ARG]\n");

		return FILEMONITOR_FAILURE_IS_NOT_ROOT;
	}

	if (argc != 2) {
		fprintf(stderr, "FileMonitor expects a max of one option only\n");
		return FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS;
	}

	enum filemonitor_error_code result = FILEMONITOR_CLEAR;
	char *option = argv[1];
	if (strcmp(option, "--run") == 0) {
		result = filemonitor_run();
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
		result = filemonitor_edit_config();
		if (result != FILEMONITOR_SUCCESS) {
			goto cleanup;
		}
	} else if (strcmp(option, "--error-codes") == 0) {
		filemonitor_print_error_codes();
	} else if (strcmp(option, "--error-log") == 0) {
		filemonitor_view_error_log();
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
	filemonitor_print_to_error_log(result);
	return FILEMONITOR_FAILURE;
}

bool is_running_as_root(void)
{
	if (geteuid() == 0)
	{
		return true; // Success: Running as root
	} else
	{
		return false; // Failure: Not running as root
	}
}

enum filemonitor_error_code filemonitor_run(void)
{
	struct config_data *data = filemonitor_get_config();
	if (data == NULL) {
		return FILEMONITOR_RUN_GET_CONFIG_FAILED;
	}

	return FILEMONITOR_SUCCESS;
}

struct config_data *filemonitor_get_config(void)
{
	return NULL;
}

void filemonitor_print_to_error_log(enum filemonitor_error_code result)
{
	FILE *error_log = NULL;

	if ((error_log = fopen("/var/log/FileMonitor/FileMonitor_error.log", "a")) == NULL) {
		perror("error opening FileMonitor_error.log for appending");
		return;
	}

	fprintf(error_log, "FileMonitor finished with error code %d.\n run \"FileMonitor --error-codes\" for more details.\n", result);

	if (fclose(error_log) == EOF) {
		perror("Error closing FileMonitor_error.log");
		return;
	}
}

enum filemonitor_error_code filemonitor_check_config(void)
{
	FILE *config_in = NULL;
	FILE *config_out = NULL;

	config_in = fopen(CONFIG_PATH, "r");
	if (config_in == NULL) {
		fprintf(stderr, "FileMonitor: Error: Could not open %s for reading: %s\n", CONFIG_PATH, strerror(errno));
		return FILEMONITOR_FAILURE_CHECK_CONFIG_FOPEN_FAILED;
	}

	config_out = fopen(TEMP_CONFIG_PATH, "w");
	if (config_out == NULL) {
		fprintf(stderr, "FileMonitor: Error: Could not open temp file %s for writing: %s\n", TEMP_CONFIG_PATH, strerror(errno));
		fclose(config_in);
		return FILEMONITOR_FAILURE_CHECK_CONFIG_FOPEN_FAILED;
	}
	
	char line[LINE_MAX];
	enum filemonitor_error_code result = FILEMONITOR_CLEAR;
	int line_num = 0;
	int lines_removed = 0;

	while (fgets(line, LINE_MAX, config_in) != NULL) {
		line_num++;
		
		if (strchr(line, '\n') == NULL && !feof(config_in)) {
			fprintf(stderr, "Line %d exceeds buffer size (%d) and will be removed.\n", line_num, LINE_MAX);
			lines_removed++;
			
			char discard[LINE_MAX];
			while (strchr(line, '\n') == NULL && fgets(discard, LINE_MAX, config_in) != NULL)
				;
			continue;
		}

		if (is_valid_config_line(line)) {
			if (fputs(line, config_out) == EOF) {
				fprintf(stderr, "FileMonitor: Error: Failed to write to temp config file.\n");
				result = FILEMONITOR_FAILURE_CHECK_CONFIG_FPUTS_FAILED;
				break;
			}
		} else {
			fprintf(stderr, "FileMonitor: Line %d removed due to bad formatting: %s", line_num, line);
			lines_removed++;
		}
	}

	fclose(config_in);
	fclose(config_out);

	/* Replace original file if processing was successful */
	if (result == FILEMONITOR_SUCCESS) {
		if (rename(TEMP_CONFIG_PATH, CONFIG_PATH) != 0) {
			fprintf(stderr, "FileMonitor: Error: Failed to replace original config file with cleaned version: %s\n", strerror(errno));
			/* Attempt to remove temp file, but keep the original */
			remove(TEMP_CONFIG_PATH);
			result = FILEMONITOR_FAILURE_CHECK_CONFIG_RENAME_FAILED;
		} else {
			printf("Configuration check complete.\n");
			if (lines_removed > 0) {
				printf("Successfully removed %d invalid lines from %s.\n", lines_removed, CONFIG_PATH);
			} else {
				printf("%s is clean.\n", CONFIG_PATH);
			}
		}
	} else {
		/* If an error occurred during reading/writing, remove the temporary file and keep the original */
		remove(TEMP_CONFIG_PATH);
		fprintf(stderr, "FileMonitor: Configuration check failed before completion. Original file retained.\n");
	}

	return result;
}

bool is_valid_config_line(const char *line)
{
	/* Trim leading whitespace (important for checking if it's an empty line or comment) */
	while (*line == ' ' || *line == '\t') {
		line++;
	}

	/* Check for empty line or comment line (valid) */
	if (*line == '\0' || *line == '\n' || *line == '#') {
		return true;
	}

	/* 2. Check for the '=' sign */
	const char *equals_sign = strchr(line, '=');
	if (equals_sign == NULL) {
		/* Non-empty, non-comment line without '=' */
		return false;
	}

	/* 3. Check for space before '=' (Rule: Eliminate spaces before and after the equals sign) */
	if (equals_sign > line && (*(equals_sign - 1) == ' ' || *(equals_sign - 1) == '\t')) {
		fprintf(stderr, "Invalid line: Space found before '=': %s", line);
		return false;
	}

	/* 4. Check for space after '=' (Rule: Eliminate spaces before and after the equals sign) */
	if (*(equals_sign + 1) == ' ' || *(equals_sign + 1) == '\t') {
		fprintf(stderr, "Invalid line: Space found after '=': %s", line);
		return false;
	}

	return true;
}

enum filemonitor_error_code filemonitor_check_log(void)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		return FILEMONITOR_FAILURE_CHECK_LOG_FORK_FAILED;
	} else if (pid == 0) {
		execlp("vim", "vim", LOG_PATH, (char *)NULL);
		perror("execlp failed to launch vim");
    		exit(FILEMONITOR_FAILURE_CHECK_LOG_VIM_NOT_INSTALLED);
	} else {
		pid_t terminated_pid = waitpid(pid, NULL, 0);
		if (terminated_pid == -1) {
			return FILEMONITOR_FAILURE_EDIT_CONFIG_WAITPID_FAILED;
		}
	}

	return FILEMONITOR_SUCCESS;
}

enum filemonitor_error_code filemonitor_edit_config(void)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		return FILEMONITOR_FAILURE_EDIT_CONFIG_FORK_FAILED;
	} else if (pid == 0) {
		execlp("vim", "vim", CONFIG_PATH, (char *)NULL);
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

void filemonitor_print_help(void)
{
	printf("FileMonitor: Help Page\n");
	printf("Options:\n");
	printf("\t--check-config\tChecks the config file to\n");
	printf("\t\t\tMake sure the formatting is correct.\n");
	printf("\t\t\tNB. It will remove any bad formatting.\n");
	printf("\t--check-log\tEnters a menu to choose a\n");
	printf("\t\t\tlog file then prints the choice to\n");
	printf("\t\t\tscreen.\n");
	printf("\t--edit-config\tModifies the config file.\n");
	printf("\t--error-codes\tDisplays the error code page for\n");
	printf("\t\t\thandy troubleshooting.\n");
	printf("\t--help\t\tBrings up this help page.\n");
	printf("\t--run\t\tRuns FileMonitor.\n");
	printf("\t--version\tPrints the current version of FileMonitor.\n");
}

void filemonitor_print_error_codes(void)
{
	printf("FileMonitor: Error Codes Page\n");
	printf("\t%d\t= SUCCESS\n", FILEMONITOR_SUCCESS);
	printf("\t%d\t= Encountered a general failure.\n", FILEMONITOR_FAILURE);
	printf("\t%d\t= FileMonitor is not being run with sufficient privileges\n", FILEMONITOR_FAILURE_IS_NOT_ROOT);
	printf("\t%d\t= Received the wrong number of arguments.\n", FILEMONITOR_FAILURE_WRONG_NUMBER_OF_ARGUMENTS);
	printf("\t%d\t= Received an unknown option.\n", FILEMONITOR_FAILURE_UNKNOWN_OPTION);
	printf("\t%d\t= Could not fork a new process when attempting to edit FileMonitor.conf in function filemonitor_edit_config\n", FILEMONITOR_FAILURE_EDIT_CONFIG_FORK_FAILED);
	printf("\t%d\t= waitpid() failed in function filemonitor_edit_config()\n", FILEMONITOR_FAILURE_EDIT_CONFIG_WAITPID_FAILED);
	printf("\t%d\t= Could not find Vim in function filemonitor_edit_config()\n", FILEMONITOR_FAILURE_EDIT_CONFIG_VIM_NOT_INSTALLED);
	printf("\t%d\t= fopen() failed to open FileMonitor.conf for read/write in function filemonitor_check_config(). File does not exist.\n", FILEMONITOR_FAILURE_CHECK_CONFIG_FOPEN_FAILED);
	printf("\t%d\t= fputs() failed to write to FileMonitor.conf.tmp in function filemonitor_check_config()\n", FILEMONITOR_FAILURE_CHECK_CONFIG_FPUTS_FAILED);
	printf("\t%d\t= rename() failed to rename FileMonitor.conf.tmp to FileMonitor.conf in function filemonitor_check_config()\n", FILEMONITOR_FAILURE_CHECK_CONFIG_RENAME_FAILED);
	printf("\t%d\t= Could not fork a new process when attempting to edit FileMonitor.log in function filemonitor_check_log()\n", FILEMONITOR_FAILURE_CHECK_LOG_FORK_FAILED);
	printf("\t%d\t= Could not find Vim in function filemonitor_check_log()\n", FILEMONITOR_FAILURE_CHECK_LOG_VIM_NOT_INSTALLED);
	printf("\t%d\t= waitpid() failed in function filemonitor_check_log()\n", FILEMONITOR_FAILURE_CHECK_LOG_WAITPID_FAILED);
	printf("\t%d\t= filemonitor_get_config() failed in function filemonitor_run()\n", FILEMONITOR_RUN_GET_CONFIG_FAILED);
	printf("\t%d\t= fork() failed in function filemonitor_view_error_log()\n", FILEMONITOR_FAILURE_VIEW_ERROR_LOG_FORK_FAILED);
	printf("\t%d\t= waitpid() failed in function filemonitor_view_error_log()\n", FILEMONITOR_FAILURE_VIEW_ERROR_LOG_WAITPID_FAILED);
}

enum filemonitor_error_code filemonitor_view_error_log(void)
{
	pid_t pid;
	pid = fork();
	if (pid < 0) {
		return FILEMONITOR_FAILURE_VIEW_ERROR_LOG_FORK_FAILED;
	} else if (pid == 0) {
		execlp("less", "less", "+G", ERROR_LOG_PATH, (char *)NULL);
		perror("execlp failed to launch less");
    		exit(FILEMONITOR_FAILURE_VIEW_ERROR_LOG_LESS_NOT_INSTALLED);
	} else {
		pid_t terminated_pid = waitpid(pid, NULL, 0);
		if (terminated_pid == -1) {
			return FILEMONITOR_FAILURE_VIEW_ERROR_LOG_WAITPID_FAILED;
		}
	}

	printf("Finished viewing FileMonitor_error.log\n");
	
	return FILEMONITOR_SUCCESS;
}

void filemonitor_print_version(void)
{
	printf("FileMonitor Version = 1.0.0.0\n");
	printf("Download and install the latest version with:\n");
	printf("git clone https://github.com/networktomp-dev/FileMonitor.git\n");
	printf("cd FileMonitor\n");
	printf("git submodule update --init --recursive\n");
	printf("make\n");
}
