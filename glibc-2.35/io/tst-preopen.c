/* Test for preopen mechanism (internal test).
   Copyright (C) 2022 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */

#include <array_length.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "preopen.h"

/* Setup test environment: create directories and files */
static int setup_test_environment(void) {
	const char* test_dirs[] = {
		"/tmp/preopen-test",
		"/tmp/preopen-test/file",
		"/tmp/preopen-test/hoge",
		"/tmp/preopen-test/huga",
		"/tmp/preopen-test/huga/piyo",
		"/tmp/preopen-test/misc",
		"/tmp/preopen-test/misc/file"
	};

	for (size_t i = 0; i < array_length(test_dirs); i++) {
		if (mkdir(test_dirs[i], 0755) < 0 && errno != EEXIST) {
			printf("Warning: Failed to create directory %s: %s\n", test_dirs[i], strerror(errno));
		}
	}

	const char* test_files_to_create[] = {
		"/tmp/preopen-test/file/sample.txt",
		"/tmp/preopen-test/sample.txt",
		"/tmp/preopen-test/huga/piyo/sample.txt",
		"/tmp/preopen-test/misc/file/sample.txt"
	};

	for (size_t i = 0; i < array_length(test_files_to_create); i++) {
		int fd = open(test_files_to_create[i], O_WRONLY | O_CREAT, 0644);
		if (fd < 0) {
			printf("Warning: Failed to create file %s: %s\n", test_files_to_create[i], strerror(errno));
		} else {
			close(fd);
		}
	}

	printf("Test environment setup completed.\n");
	return 0;
}

/* Test __preopen directly */
static int test_preopen_direct(const char *file, int oflag, int mode, bool expect_success) {
	printf("Testing __preopen(\"%s\", %d, %d) ... ", file, oflag, mode);

	int res = __preopen(file, oflag, mode);

	if (res > 0) {
		printf("success: fd=%d", res);
		if (expect_success) {
			printf(" [PASS]\n");
			return 0;
		} else {
			printf(" [FAIL: expected failure]\n");
			return -1;
		}
	} else {
		printf("failed: %d", res);
		if (!expect_success) {
			printf(" [PASS: expected failure]\n");
			return 0;
		} else {
			printf(" [FAIL: expected success]\n");
			return -1;
		}
	}
}

static int do_test (void) {
	setup_test_environment();

	const char* test_files[] = {
		"/tmp/preopen-test/file/sample.txt",
		"/tmp/preopen-test/misc/file/sample.txt"
	};
	const char* test_path[] = {
		"/tmp/preopen-test",
		"/tmp/preopen-test/hoge",
		"/tmp/preopen-test/huga",
		"/tmp/preopen-test/huga/piyo",
		"/tmp/preopen-test/misc"
	};

	int fails = 0;
	int fd;
	size_t i;
	char path_fd_str[256];
	char path_str[1024];
	char file_fd_str[256];
	char file_str[1024];
	int path_fd_pos = 0;
	int path_str_pos = 0;
	int file_fd_pos = 0;
	int file_str_pos = 0;

	char pathname[512];
	memset(pathname, '\0', sizeof(pathname));
	getcwd(pathname, sizeof(pathname));
	printf("Current directory: %s\n", pathname);

	/* Open directories and build environment variables */
	for (i = 0; i < array_length(test_path); i++) {
		fd = open(test_path[i], O_DIRECTORY, 0644);
		printf("path index: %zu, Open fd: %d\n", i, fd);
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

	/* Open files and build environment variables */
	for (i = 0; i < array_length(test_files); i++) {
		fd = open(test_files[i], O_RDWR, 0644);
		printf("file index: %zu, Open fd: %d\n", i, fd);
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

	int tmp_fd = open("/tmp/test-preopen-log", O_WRONLY | O_APPEND | O_CREAT, 0644);
	if (tmp_fd < 0) {
		printf("Warning: Failed to create debug log file\n");
	}

	char tmp_fd_str[32];
	snprintf(tmp_fd_str, sizeof(tmp_fd_str), "%d", tmp_fd);

	/* Set environment variables */
	setenv("PREOPEN_DEBUG_FD", tmp_fd_str, 1);
	setenv("PREOPEN_PATHS", path_str, 1);
	setenv("PREOPEN_PATH_FDS", path_fd_str, 1);
	setenv("PREOPEN_FILES", file_str, 1);
	setenv("PREOPEN_FILE_FDS", file_fd_str, 1);

	printf("\n=== Environment Variables ===\n");
	printf("PREOPEN_PATHS: %s\n", path_str);
	printf("PREOPEN_PATH_FDS: %s\n", path_fd_str);
	printf("PREOPEN_FILES: %s\n", file_str);
	printf("PREOPEN_FILE_FDS: %s\n", file_fd_str);
	printf("\n=== Direct __preopen() Tests ===\n");

	/* Tests expecting success (PREOPEN_PATHS match) */
	fails |= test_preopen_direct("/tmp/preopen-test/sample.txt", O_RDONLY, 0, true);
	fails |= test_preopen_direct("/tmp/preopen-test/huga/piyo/sample.txt", O_RDONLY, 0, true);

	/* Tests expecting success (PREOPEN_FILES match) */
	fails |= test_preopen_direct("/tmp/preopen-test/file/sample.txt", O_RDONLY, 0, true);
	fails |= test_preopen_direct("/tmp/preopen-test/misc/file/sample.txt", O_RDONLY, 0, true);

	/*Test Relative path */
	fails |= test_preopen_direct("sample.txt", O_RDONLY, 0, true);
	/* Tests expecting failure (no match) */
	fails |= test_preopen_direct("/null/hoge.txt", O_RDONLY, 0, false);
	fails |= test_preopen_direct("/etc/passwd", O_RDONLY, 0, false);

	/* Relative path tests */
	printf("\n=== Relative Path Tests (may depend on cwd) ===\n");
	int rel_result = __preopen("file/sample.txt", O_RDONLY, 0);
	printf("__preopen(\"file/sample.txt\") = %d\n", rel_result);

	printf("\n=== Test Summary ===\n");
	if (fails) {
		printf("Some tests FAILED\n");
	} else {
		printf("All tests PASSED\n");
	}

	return fails;
}

#include "support/test-driver.c"
