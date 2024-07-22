#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define PATH_MAX_LEN 128

// open path from library list split with ":".
// success: return string FDs with split ":"
// error: return empty string
char* open_lib_path(char** lib_path, int n_lib_path){
	char* env_lib_fds = malloc(PATH_MAX_LEN);
	int i;
	int lib_fds[n_lib_path];

	printf("[DEBUG] file open in open_lib_path(), environ name: LD_LIBRARY_PATH_FDS\n");
	for ( i = 0; i < n_lib_path; i++){
		if (lib_path[i] != NULL) {
			int fd = openat(AT_FDCWD, lib_path[i], O_RDONLY);
			if (fd < 0){

				printf(" | [failed] cannot open path: %s\n", lib_path[i]);
				lib_path[i] = NULL;
			} else {
				printf(" | [success] openat path:%s, fd:%d\n", lib_path[i], fd);
				lib_fds[i] = fd;
			}
		}
	}
	int offset = snprintf(env_lib_fds, 3, "%d", lib_fds[0]);
	for (i = 1; i < n_lib_path; i++){
		if (lib_path[i] != NULL) offset += snprintf(env_lib_fds+offset, 4, ":%d", lib_fds[i]);
	}

	printf("========\n");
	return env_lib_fds;
}

void main(){
	extern char **environ;

	char *binary = "/bin/echo";
	char *rtld = "./local/lib/ld-linux-x86-64.so.2";
	char *argv[] = {rtld, "/bin/echo", "hello", NULL};

	char *libs[4];
	int i;
	for(i = 0; i < 4; i++) {
		libs[i] = calloc(PATH_MAX_LEN, sizeof(char));
		libs[i] = NULL;
	}
	libs[0] = "/lib32";
	libs[1] = "/usr/lib";
	libs[2] = "/usr/local/lib";
	libs[3] = "/tmp";
	char* lib_fds = open_lib_path(libs, 4);
	setenv("LD_LIBRARY_PATH_FDS", lib_fds, 1);
	
	int test_fd = openat(3, "libc.so.6", O_RDONLY);
	printf("opened test_fd: %d\n", test_fd);

	execve(rtld, argv, environ);
}
