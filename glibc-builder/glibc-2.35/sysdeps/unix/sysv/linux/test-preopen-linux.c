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
	if (res > 0) {
		printf("open succsess: %d\n",res);
		return 0;
	}
	else {
		printf("open failed: %d\n",res);
		return -1;
	}
}

static int do_test (void) {
	const char* test_files[] = {"/tmp/file/sample.txt", "./file/sample.txt"};
	const char* test_path[] = {"/tmp", "/tmp/hoge", "/tmp/huga","/tmp/huga/piyo", "/home/materialofmouse/await-glibc/glibc-builder/glibc-2.35/misc"};
	int fails = 0;
	int fd, i;
	char path_fd_str[256];
	char path_str[1024];
	char file_fd_str[256];
	char file_str[1024];
	int path_fd_pos = 0;
	int path_str_pos = 0;
	int file_fd_pos = 0;
	int file_str_pos = 0;

	int PATHNAME_SIZE = 512;
	// 変数定義
	char pathname[PATHNAME_SIZE];  // ファイルパス
	// 変数初期化
	memset(pathname, '\0', PATHNAME_SIZE);

	// カレントディレクトリ取得
	getcwd(pathname, PATHNAME_SIZE);
	fprintf(stdout,"現在のファイルパス:%s\n", pathname);

	// パスの処理
	for (i = 0; i < sizeof(test_path) / sizeof(test_path[0]); i++) {
		fd = open(test_path[i], O_DIRECTORY, 0644);
		printf("path index: %d, Open fd: %d\n", i, fd);
		if (fd < 0) {
			printf("Failed to open path %s: %s\n", test_path[i], strerror(errno));
			return 1;
		}
		if (i > 0) {
			path_fd_pos += snprintf(path_fd_str + path_fd_pos, sizeof(path_fd_str) - path_fd_pos, ":");
			path_str_pos += snprintf(path_str + path_str_pos, sizeof(path_str) - path_str_pos, ":");
		}
		path_fd_pos += snprintf(path_fd_str + path_fd_pos, sizeof(path_fd_str) - path_fd_pos, "%d", fd);
		path_str_pos += snprintf(path_str + path_str_pos, sizeof(path_str) - path_str_pos, "%s", test_path[i]);
	}

	// ファイルの処理
	for (i = 0; i < sizeof(test_files) / sizeof(test_files[0]); i++) {
		fd = open(test_files[i], O_RDWR , 0644);
		printf("file index: %d, Open fd: %d\n", i, fd);
		if (fd < 0) {
			printf("Failed to open file %s: %s\n", test_files[i], strerror(errno));
			return 1;
		}
		if (i > 0) {
			file_fd_pos += snprintf(file_fd_str + file_fd_pos, sizeof(file_fd_str) - file_fd_pos, ":");
			file_str_pos += snprintf(file_str + file_str_pos, sizeof(file_str) - file_str_pos, ":");
		}
		file_fd_pos += snprintf(file_fd_str + file_fd_pos, sizeof(file_fd_str) - file_fd_pos, "%d", fd);
		file_str_pos += snprintf(file_str + file_str_pos, sizeof(file_str) - file_str_pos, "%s", test_files[i]);
	}

	setenv("PREOPEN_PATHS", path_str, 1);
	setenv("PREOPEN_PATH_FDS", path_fd_str, 1);
	setenv("PREOPEN_FILES", file_str, 1);
	setenv("PREOPEN_FILE_FDS", file_fd_str, 1);
	printf("PREOPEN_FILES: %s, PREOPEN_PATHS: %s\n", path_str, path_fd_str);
	printf("PREOPEN_FILE_FDS: %s, PREOPEN_PATH_FDS: %s\n", file_str, path_fd_str);
	
	fails |= test_preopen ("/tmp/sample.txt", O_RDONLY, 0, 0);
	fails |= test_preopen ("sample.txt", O_RDONLY, 0, 0);
	fails |= test_preopen ("hoge/sample.txt", O_RDONLY, 0, 0);
	fails |= test_preopen ("/tmp/huga/piyo/sample.txt", O_RDONLY, 0, 0);
	fails |= test_preopen ("/etc/hoge.txt", O_RDONLY, 0, 0);
	return fails;
}
#include "support/test-driver.c"
