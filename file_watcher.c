#include "file_watcher.h"

#define USAGE "usage: file_watcher <source_file> <destination_directory> <log_directory> <log_file_name>\n"

int main(int argc, char **argv){
    if(argc != 5){
	printf(USAGE);
	exit(1);
    }

    char src_file_name[strlen(argv[1])];
    strcpy(src_file_name, argv[1]);

    char dest_dir_name[strlen(argv[2])];
    strcpy(dest_dir_name, argv[2]);

    char log_dir_name[strlen(argv[3])];
    strcpy(log_dir_name, argv[3]);
    
    char log_file_naming[strlen(argv[4])];
    strcpy(log_file_naming, argv[4]);

    if(get_file_type(src_file_name) != FILE_T){
	printf(USAGE);
	printf("source_file, %s, does not exist\n", src_file_name);
	exit(2);
    }else if(get_file_type(dest_dir_name) != DIRECTORY_T){
	printf(USAGE);
	printf("destination_directory, %s, does not exist\n", dest_dir_name);
	exit(3);
    }else if(get_file_type(log_dir_name) != DIRECTORY_T){
	printf(USAGE);
	printf("log_directory,  does not exist\n");
	exit(4);
    }

    pid_t child_pid;

    child_pid = fork();

    if(child_pid >= 0){
	if(child_pid == 0){
	    setsid();
	    child_pid = fork();

	    if(child_pid >= 0){
		if(child_pid == 0){
		    watch_file(src_file_name, dest_dir_name, log_dir_name, log_file_naming);
		}
	    }else{
		printf("2nd fork has failed\n");
	    }
	}
    }else{
	printf("Failed to fork\n");
    }

    return 0;
}

void watch_file(char *src_file_name, char *dest_dir_name, char *log_dir_name, char *log_file_naming){

    int log_path_length = strlen(log_dir_name) + 1 + strlen(log_file_naming) + 4;
    char log_out_path[log_path_length], log_err_path[log_path_length];

    strcpy(log_out_path, log_dir_name);
    strcat(log_out_path, "/");
    strcat(log_out_path, log_file_naming);
    strcat(log_out_path, ".out");

    strcpy(log_err_path, log_dir_name);
    strcat(log_err_path, "/");
    strcat(log_err_path, log_file_naming);
    strcat(log_err_path, ".err");

/*
    FILE *log_out_file = fopen(log_out_path, "w+");
    if(log_out_file == NULL){
	printf("Failed to create log out file: %s\n", log_out_path);
	perror("Failed to create log out file");
	exit(10);
    }

    FILE *log_err_file = fopen(log_err_path, "w+");
    if(log_err_file == NULL){
	printf("Failed to create log error file: %s\n", log_err_path);
	exit(11);
    }
*/
    int log_out_fd = open(log_out_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    // int log_out_fd = fileno(log_out_file);
    if(log_out_fd == -1){
	printf("Failed to get file descriptor for file: %s\n", log_out_path);
	exit(12);
    }

    printf("Opening log.out file: %s\n", log_out_path);

    int log_err_fd = open(log_err_path, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    //int log_err_fd = fileno(log_err_file);
    if(log_err_fd == -1){
	printf("Failed to get file descriptor for file: %s\n", log_err_path);
	exit(12);
    }

    printf("Opening log.err file: %s\n", log_err_path);

    if(dup2(log_out_fd, STDOUT_FILENO) == -1){
	printf("Failed to redirect standard out to file: %s\n", log_out_path);
	exit(13);
    }

    printf("Redirecting standard out to file: %s\n", log_out_path);

    if(dup2(log_err_fd, STDERR_FILENO) == -1){
	printf("Failed to redirect standard error to file: %s\n", log_err_path);
	exit(14);
    }

    printf("Redirecting standard error to file: %s\n", log_err_path);
    
    close(STDIN_FILENO);
    close(log_out_fd);
    close(log_err_fd);

    char src_file_abs_path[PATH_MAX];

    if(realpath(src_file_name, src_file_abs_path) == NULL){
	printf("Failed to find absolute path for source file: %s\n", src_file_name);
	exit(17);
    }

    printf("Source file found: %s\n", src_file_abs_path);

    char *dest_file_name;
    get_file_name(src_file_name, &dest_file_name);

    char dest_path_name[strlen(dest_dir_name) + 1 + strlen(dest_file_name)];

    strcpy(dest_path_name, dest_dir_name);
    strcat(dest_path_name, "/");
    strcat(dest_path_name, dest_file_name);

    FILE *dest_file = fopen(dest_path_name, "w+");
    if(dest_file == NULL){
        printf("Failed to create destination file: %s\n", dest_path_name);
        exit(9);
    }

    fclose(dest_file);

    char dest_file_abs_path[PATH_MAX];
    if(realpath(dest_path_name, dest_file_abs_path) == NULL){
	printf("Failed to find absolute path for destination file: %s\n", dest_path_name);
	exit(99);
    }

    printf("Destination file found: %s\n", dest_file_abs_path);

    chdir("/");
    umask(0);

    while(1){
	time_t raw_time;
	struct tm *time_info;

	time(&raw_time);
	time_info = localtime(&raw_time);

	int sleep_sec = (22 * 3600 + 60 * 5) - (time_info->tm_hour * 3600 + time_info->tm_min * 60 + time_info->tm_sec);

	if(sleep_sec < 0){
	    sleep_sec = 24 * 3600 + sleep_sec;
	}

	sleep(sleep_sec);

	FILE *src_file = fopen(src_file_abs_path, "r");
	if(src_file == NULL){
	    printf("Failed to find source file: %s\n", src_file_abs_path);
	    exit(8);
	}

	dest_file = fopen(dest_file_abs_path, "w");
	if(dest_file == NULL){
	    printf("Failed to open destination file: %s\n", dest_file_name);
	    exit(9);
	}

	size_t bytes_read, total_bytes_read = 0;
	char buffer[1024];
	while((bytes_read = fread(buffer, 1, sizeof buffer, src_file)) > 0){
	    fwrite(buffer, 1, bytes_read, dest_file);

	    if(ferror(dest_file) != 0){
		break;
	    }
	    total_bytes_read += bytes_read;
	}

	printf("Bytes read: %lu\n", total_bytes_read);

	fclose(src_file);
	fclose(dest_file);

	sleep(60);
    }

    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // fclose(log_out_file);
    // fclose(log_err_file);
}

enum file_type get_file_type(const char *file_path){
    struct stat path_stat;
    stat(file_path, &path_stat);

    
    if(S_ISREG(path_stat.st_mode)){
	return FILE_T;
    }else if(S_ISDIR(path_stat.st_mode)){
	return DIRECTORY_T;
    }else{
	return UNKNOWN_T;
    }
}

int get_file_name(const char *file_path, char **file_name){
    int i = 0, start_file_name = -1;
    char c;
    while((c = file_path[i]) != '\0'){
	if(c == '/'){
	    start_file_name = i;
	}
	i++;
    }

    if(start_file_name == -1){
	strcpy(*file_name, file_path);
	return 0;
    }

    size_t file_name_length = sizeof(char) * (strlen(file_path) - start_file_name);
    
    printf("file_name_length: %lu\n", file_name_length);

    *file_name = malloc(file_name_length);

    memcpy(*file_name, &file_path[start_file_name + 1], file_name_length);

    printf("file_name: %s\n", *file_name);

    return 0;
}
