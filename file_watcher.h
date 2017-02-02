#ifndef FILE_WATCHER_H
#define FILE_WATCHER_H

#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>

#include "error_handler.h"

enum file_type {
	FILE_T,
	DIRECTORY_T,
	UNKNOWN_T
};

struct file_watcher_t {
	char *src_file_name;
	char *dest_dir_name;
	size_t interval;
};

int get_file_name(const char *file_path, char **file_name);

void watch_file(char *src_file_name, char *dest_dir_name, char *log_dir_name, char *log_file_naming);

enum file_type get_file_type(const char* file_path);

#endif // FILE_WATCHER_H
