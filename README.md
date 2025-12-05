FileMonitor

A basic CLI utility to monitor excessively large files or directories
with too many files in it. Perfect for monitoring a server where
there are many users.

------
install with:
git clone https://github.com/networktomp-dev/FileMonitor.git
cd FileMonitor
git submodule update --init --recursive
make

------

It is designed to be run via a Cronjob and must have root privileges.

It will write suspicious activity to a .log file for review upon login.
Email notifications would be great too.

It uses a .conf file to read configurations.
For example:
###                         ###
### FileMonitor Config File ###
###                         ###

### Rules ###
# Comments must be on a separate line
# Eliminate spaces before and after the equals sign
# Filepaths must be full paths starting with /

### Admin details ###
# Name of Administrator. This is for address only.
admin=networktomp
# Choose an email for notifications to be sent to for remote system
# auditing
adminemail=networktomp@webmail.com

### General Size Monitoring ###
# Set the max size a file can be before it is flagged by FileMonitor
file_max_size=5368709120
# Set the max size a directory can be before it is flagged by
# FileMonitor
dir_max_sizesize=1000
# Set files and directories to ignore. Use full filepath
ignore_dir=/root
ignore_dir=/tmp
ignore_file=*.mp4
ignore_file=/home/user/src/really_important_project.c

### Individual File Monitoring ###
# Choose a specific file or files for auditing. Log files are kept in
# FileMonitor/log
monitor_file=/etc/passwd

### Logging ###
# Choose how big a log file should grow before it is moved to FileMonitor/tmp
log_max_size=1048576
# Choose how long a log file should remain in FileMonitor/tmp before it
# is deleted. Measured in days
log_retention=30
