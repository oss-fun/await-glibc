#include <array_length.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include <sysdeps/unix/sysv/linux/open64.c>
//extern int preopen(const char *file, int oflag, int mode, int *last_errno);

static int test_preopen(const char *file, int oflag, int mode, int *last_errno) {
	int res = open(file, oflag, mode, last_errno);
	if (res > 0) return 0;
	else return -1;
}


static int do_test (void) {
	int fails = 0;
	const char* test_path[] = {"/tmp", "/tmp/hoge", "/tmp/huga", "/tmp/sample.txt"};
	int fd, i;
	char fd_str[256];
	char path_str[1024];
	int fd_str_pos = 0;
	int path_str_pos = 0;

	for (i = 0; i < sizeof(test_path) / sizeof(test_path[0]); i++) {
		fd = open(test_path[i], O_RDWR | O_CREAT, 0644);
		if (fd < 0) {
			printf("Failed to open %s: %s\n", test_path[i], strerror(errno));
			return 1;
		}

		if (i > 0) {
			fd_str_pos += snprintf(fd_str + fd_str_pos, sizeof(fd_str) - fd_str_pos, ":");
		}
		fd_str_pos += snprintf(fd_str + fd_str_pos, sizeof(fd_str) - fd_str_pos, "%d", fd);

		if (i > 0) {
			path_str_pos += snprintf(path_str + path_str_pos, sizeof(path_str) - path_str_pos, ":");
		}
		path_str_pos += snprintf(path_str + path_str_pos, sizeof(path_str) - path_str_pos, "%s", test_path[i]);
	}

	setenv("PREOPEN_PATHS", path_str, 1);
	setenv("PREOPEN_FDS", fd_str, 1);

	fails |= test_preopen ("/tmp/sample.txt", O_RDONLY, 0, 0);
	fails |= test_preopen ("sample.txt", O_RDONLY, 0, 0);
	fails |= test_preopen ("hoge/sample.txt", O_RDONLY, 0, 0);
	fails |= test_preopen ("/etc/passwd", O_RDONLY, 0, 0);
	return fails;
}
#include "support/test-driver.c"
