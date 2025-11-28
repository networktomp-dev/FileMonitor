FileMonitor

A basic CLI utility to monitor excessively large files or directories
with too many files in it. Perfect for monitoring a server where
there are many users.

It is designed to be run via a Cronjob and must have root privileges.

It will write suspicious activity to a .log file for review upon login.
Email notifications would be great too.

It uses a .conf file to read configurations.
For example:
filesize=5GB # Flags file sizes above 5GB
dirsize=1000 # Flags directory sizes above 1000 files (shallow)
admin=networktomp # Administrator's name
adminemail=networktomp@webmail.com # email to send notifications to
ignoredir=/root # set directories to ignore (recursively)
ignoredir=/sbin
ignorefile=*.mp4 # Set files to ignore
ignorefile=/home/user/src/really_important_project.c
