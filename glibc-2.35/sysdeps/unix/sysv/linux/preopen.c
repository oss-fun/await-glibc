/* Preopen mechanism for restricted file access.
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

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <sysdep-cancel.h>

#include "preopen.h"

/* Internal debug printf function */
__attribute__((unused))
static void debug_printf(const char *format, ...) {
	char buffer[1024];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	size_t len = strlen(buffer);
	INLINE_SYSCALL_CALL(write, 2, buffer, len);
}

#ifdef PREOPEN_DEBUG
#define DEBUG_PRINTF(msg, ...) debug_printf(msg, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(msg, ...) ((void)0)
#endif

/* Get debug output file descriptor from environment.
   Checks PREOPEN_DEBUG_FD first, then PREOPEN_DEBUG_FILE. */
int __preopen_get_debug_fd(void) {
	/* Check if FD is specified in environment */
	const char *fd_str = getenv("PREOPEN_DEBUG_FD");
	if (fd_str) {
		char *endptr;
		long fd = strtol(fd_str, &endptr, 10);
		if (*endptr == '\0' && fd > 0) {
			return (int)fd;
		}
	}

	/* If FD not specified, try to open from path */
	const char *debug_log_path = getenv("PREOPEN_DEBUG_FILE");
	if (!debug_log_path) {
		return -1;
	}

	return SYSCALL_CANCEL(open, debug_log_path, O_WRONLY | O_APPEND | O_CREAT, 0666);
}

/* Write debug log entry to file */
void __preopen_debug_log(int log_fd, const char *prefix, const char *filepath, int fd) {
	if (log_fd <= 0) return;

	char buffer[4096] = {0};
	int len = 0;

	/* prefix + ": " */
	int prefix_len = strlen(prefix);
	memcpy(buffer + len, prefix, prefix_len);
	len += prefix_len;
	memcpy(buffer + len, ": ", 2);
	len += 2;

	/* filepath */
	int path_len = strlen(filepath);
	memcpy(buffer + len, filepath, path_len);
	len += path_len;

	/* status (with space) */
	if (fd > 0) {
		memcpy(buffer + len, " success", 8);
		len += 8;
	} else {
		memcpy(buffer + len, " failed", 7);
		len += 7;
	}

	/* newline */
	buffer[len] = '\n';
	len += 1;

	/* single write */
	INLINE_SYSCALL_CALL(write, log_fd, buffer, len);
}

/* Remove the last path component.
   Example: /a/b/c.txt -> /a/b */
static char *remove_last_component(const char *path) {
	if (!path) return NULL;

	/* Allocate buffer for result */
	char *result = strdup(path);
	if (!result) return NULL;

	/* Remove trailing slashes */
	size_t len = strlen(result);
	while (len > 0 && result[len - 1] == '/') {
		result[--len] = '\0';
	}

	/* Find last '/' */
	char *last_slash = strrchr(result, '/');
	if (last_slash) {
		*last_slash = '\0';
		/* Keep '/' for root directory */
		if (last_slash == result) {
			result[1] = '\0';
		}
	} else {
		/* No slash found, return empty string */
		result[0] = '\0';
	}

	return result;
}

/* Convert relative path to absolute path */
static char *get_absolute_path(const char *path) {
	char cwd[PATH_MAX];
	char *abs_path = malloc(PATH_MAX);

	if (!abs_path) return NULL;
	memset(abs_path, 0, PATH_MAX);

	/* Already absolute path */
	if (path[0] == '/') {
		strncpy(abs_path, path, PATH_MAX - 1);
		abs_path[PATH_MAX - 1] = '\0';
		return abs_path;
	}

	/* Get current directory */
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		free(abs_path);
		return NULL;
	}

	/* Handle "./" prefix */
	const char *path_to_append;
	if (strncmp(path, "./", 2) == 0) {
		path_to_append = path + 2;
	} else {
		path_to_append = path;
	}

	size_t cwd_len = strlen(cwd);
	size_t path_len = strlen(path_to_append);

	/* Check buffer size (+2 for '/' and '\0') */
	if (cwd_len + path_len + 2 > PATH_MAX) {
		free(abs_path);
		return NULL;
	}

	/* Build string using memcpy */
	memcpy(abs_path, cwd, cwd_len);
	abs_path[cwd_len] = '/';
	memcpy(abs_path + cwd_len + 1, path_to_append, path_len);
	abs_path[cwd_len + path_len + 1] = '\0';

	/* Remove trailing slash */
	size_t total_len = strlen(abs_path);
	if (total_len > 1 && abs_path[total_len - 1] == '/') {
		abs_path[total_len - 1] = '\0';
	}

	return abs_path;
}

/* Convert colon-separated environment variable string to array.
   Returns NULL-terminated array of strings. */
static char** get_string_list(char* env_str) {
	int cnt = 0;

	char** result_list = malloc(sizeof(char *) * PREOPEN_MAX_LIST_LEN);
	if (result_list == NULL) {
		return NULL;
	}

	if (env_str) {
		char *token = strtok(env_str, ":");
		while (token && cnt < PREOPEN_MAX_LIST_LEN) {
			char *buff = token;
			result_list[cnt] = strdup(buff);
			if (result_list[cnt] == NULL) {
				/* Handle memory allocation failure */
				for (int i = 0; i < cnt; i++) {
					free(result_list[i]);
				}
				free(result_list);
				return NULL;
			}
			cnt++;
			token = strtok(NULL, ":");
		}
	}

	/* NULL terminate the list */
	result_list[cnt] = NULL;
	return result_list;
}

/* Get n-th component of path (0-indexed).
   Returns allocated string on success, NULL on failure. */
static char* get_path_component(const char *path, int n) {
	char *result = malloc(sizeof(char) * PREOPEN_MAX_PATH_LEN);
	if (path == NULL) return NULL;

	const char *start = path + 1;  /* Skip first '/' */
	const char *end;
	int current = 0;

	while (current < n) {
		start = strchr(start, '/');
		if (start == NULL) {
			free(result);
			return NULL;  /* n-th component doesn't exist */
		}
		start++;  /* Skip '/' */
		current++;
	}

	end = strchr(start, '/');
	if (end == NULL) {
		/* Last component */
		if (strlen(start) >= PREOPEN_MAX_PATH_LEN) {
			free(result);
			return NULL;  /* Buffer too small */
		}
		strncpy(result, start, PREOPEN_MAX_PATH_LEN - 1);
		result[PREOPEN_MAX_PATH_LEN - 1] = '\0';
	} else {
		size_t len = end - start;
		if (len >= PREOPEN_MAX_PATH_LEN) {
			free(result);
			return NULL;  /* Buffer too small */
		}
		strncpy(result, start, len);
		result[len] = '\0';
	}

	return result;
}

/* Find matching preopen path by comparing path components.
   Returns index of matching path, or -1 if not found. */
static int find_matching_preopen_path(const char *file_path, char **preopen_paths, int path_count) {
	char *file_component;
	char *preopen_component;
	DEBUG_PRINTF("path_count: %d\n", path_count);

	for (int i = 0; i < path_count; i++) {
		int depth = 0;
		bool match = true;

		/* Compare each path component */
		while (1) {
			file_component = get_path_component(file_path, depth);
			preopen_component = get_path_component(preopen_paths[i], depth);
			DEBUG_PRINTF("index [%d] path_component: %s, preopen path component: %s\n",
			             i, file_component, preopen_component);

			if (file_component == NULL && preopen_component == NULL) break;
			if ((file_component == NULL) != (preopen_component == NULL)) {
				match = false;
				free(file_component);
				free(preopen_component);
				break;
			}
			if (strcmp(file_component, preopen_component) != 0) {
				match = false;
				free(file_component);
				free(preopen_component);
				break;
			}
			free(file_component);
			free(preopen_component);
			depth++;
		}
		if (match) {
			return i;
		}
	}

	return -1;
}

/* Try to open file using PREOPEN_FILES/PREOPEN_FILE_FDS (exact match). */
int __preopen_file(const char *file, int oflag, int mode) {
	DEBUG_PRINTF("in __preopen_file\n");
	int cnt = 0;
	char *preopen_files_str;
	char *preopen_file_fds_str;
	char **preopen_files = NULL;
	char **preopen_file_fds_list = NULL;
	int preopen_file_fds[PREOPEN_MAX_LIST_LEN];

	/* Get PREOPEN_FILES as list */
	char* buff = getenv("PREOPEN_FILES");
	if (buff != NULL) {
		DEBUG_PRINTF("PREOPEN_FILES=%s\n", buff);
		preopen_files_str = strdup(buff);
		preopen_files = get_string_list(preopen_files_str);
		free(preopen_files_str);
	} else {
		DEBUG_PRINTF("PREOPEN_FILES is not set\n");
		return -1;
	}

	/* Get PREOPEN_FILE_FDS as list and convert to int */
	buff = getenv("PREOPEN_FILE_FDS");
	if (buff != NULL) {
		DEBUG_PRINTF("PREOPEN_FILE_FDS=%s\n", buff);
		preopen_file_fds_str = strdup(buff);
		preopen_file_fds_list = get_string_list(preopen_file_fds_str);
		free(preopen_file_fds_str);
	} else {
		DEBUG_PRINTF("PREOPEN_FILE_FDS is not set\n");
		return -1;
	}

	while (*preopen_file_fds_list != NULL) {
		char *endptr;
		long int fd = strtol(*preopen_file_fds_list, &endptr, 10);
		if (*endptr != '\0') fd = -1;
		preopen_file_fds[cnt++] = (int)fd;
		preopen_file_fds_list++;
	}

	/* Path matching */
	const char *abs_file = file;
	if (file[0] != '/') abs_file = get_absolute_path(file);
	DEBUG_PRINTF("abs_file: %s\n", abs_file);

	int path_index = find_matching_preopen_path(abs_file, preopen_files, cnt);
	if (path_index >= 0) {
		DEBUG_PRINTF("preopen file match. file:%s, preopen file:%s, fd:%d\n",
		             abs_file, preopen_files[path_index], preopen_file_fds[path_index]);
		int fd = preopen_file_fds[path_index];
		DEBUG_PRINTF("file preopen successful\n");
		return fd;
	}
	DEBUG_PRINTF("file no match\n");
	return -1;
}
libc_hidden_def(__preopen_file)

/* Try to open file using PREOPEN_PATHS/PREOPEN_PATH_FDS with openat(). */
int __preopen_from_dir(const char *file, int oflag, int mode) {
	int cnt = 0;
	char *preopen_paths_str;
	char *preopen_path_fds_str;
	char **preopen_paths = NULL;
	char **preopen_path_fds_list = NULL;
	int preopen_path_fds[PREOPEN_MAX_LIST_LEN];
	DEBUG_PRINTF("in __preopen_from_dir\n");

	/* Get PREOPEN_PATHS as list */
	char *buff = getenv("PREOPEN_PATHS");
	if (buff != NULL) {
		DEBUG_PRINTF("PREOPEN_PATHS=%s\n", buff);
		preopen_paths_str = strdup(buff);
		preopen_paths = get_string_list(preopen_paths_str);
		free(preopen_paths_str);
	} else {
		DEBUG_PRINTF("PREOPEN_PATHS is not set\n");
		return -1;
	}

	/* Get PREOPEN_PATH_FDS as list and convert to int */
	buff = getenv("PREOPEN_PATH_FDS");
	if (buff != NULL) {
		DEBUG_PRINTF("PREOPEN_PATH_FDS=%s\n", buff);
		preopen_path_fds_str = strdup(buff);
		preopen_path_fds_list = get_string_list(preopen_path_fds_str);
		free(preopen_path_fds_str);
	} else {
		DEBUG_PRINTF("PREOPEN_PATH_FDS is not set\n");
		return -1;
	}

	while (*preopen_path_fds_list != NULL) {
		char *endptr;
		long int fd = strtol(*preopen_path_fds_list, &endptr, 10);
		if (*endptr != '\0') fd = -1;
		preopen_path_fds[cnt++] = (int)fd;
		preopen_path_fds_list++;
	}

	/* Path matching with absolute path */
	const char *abs_path = file;
	if (file[0] != '/') abs_path = get_absolute_path(file);
	/* Remove last component unless opening directory */
	if (!(oflag & O_DIRECTORY)) abs_path = remove_last_component(abs_path);

	DEBUG_PRINTF("preopen abs_path: %s\n", abs_path);
	int path_index = find_matching_preopen_path(abs_path, preopen_paths, cnt);
	if (path_index >= 0) {
		DEBUG_PRINTF("file match. file:%s, preopen_dir:%s, fd:%d\n",
		             abs_path, preopen_paths[path_index], preopen_path_fds[path_index]);
		int fd;
		if (oflag & O_CREAT) {
			fd = SYSCALL_CANCEL(openat, preopen_path_fds[path_index], abs_path, oflag, mode);
		} else {
			fd = SYSCALL_CANCEL(openat, preopen_path_fds[path_index], abs_path, oflag);
		}
		if (fd != -1) {
			DEBUG_PRINTF("abs preopen successful\n");
			return fd;
		}
	} else {
		DEBUG_PRINTF("not path index\n");
	}
	return -1;
}
libc_hidden_def(__preopen_from_dir)

/* Main preopen function.
   Tries __preopen_file first, then __preopen_from_dir. */
int __preopen(const char *file, int oflag, int mode) {
	DEBUG_PRINTF("---- begin __preopen\n");
	DEBUG_PRINTF("try open: %s\n", file);

	int preopen_fd;
	char *test __attribute__((unused));

	preopen_fd = __preopen_file(file, oflag, mode);
	if (preopen_fd > 0) return preopen_fd;

	preopen_fd = __preopen_from_dir(file, oflag, mode);
	if (preopen_fd > 0) {
		test = getenv("PREOPEN_PATH_FDS");
		DEBUG_PRINTF("PREOPEN_PATH_FDS:%s\n", test);
		return preopen_fd;
	}

	test = getenv("PREOPEN_PATH_FDS");
	DEBUG_PRINTF("PREOPEN_PATH_FDS:%s\n", test);
	DEBUG_PRINTF("---- end __preopen\n");
	return -1;
}
