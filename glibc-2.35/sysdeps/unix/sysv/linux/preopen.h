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

#ifndef _PREOPEN_H
#define _PREOPEN_H

#include <stddef.h>
#include <libc-symbols.h>

/* Maximum number of entries in PREOPEN environment variables */
#define PREOPEN_MAX_LIST_LEN   128
/* Maximum length of a single path */
#define PREOPEN_MAX_PATH_LEN   256

/* Main preopen function (internal).
   Attempts to open a file using preopened file descriptors.

   Arguments:
   - file:   Path to the file to open
   - oflag:  Open flags (O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, etc.)
   - mode:   File permissions when O_CREAT is specified

   Returns:
   - On success: file descriptor (positive integer)
   - On failure: -1 (no matching preopen path found)

   Environment variables used:
   - PREOPEN_PATHS:    Colon-separated list of preopened directory paths
   - PREOPEN_PATH_FDS: Colon-separated list of FDs for PREOPEN_PATHS
   - PREOPEN_FILES:    Colon-separated list of preopened file paths
   - PREOPEN_FILE_FDS: Colon-separated list of FDs for PREOPEN_FILES
*/
extern int __preopen(const char *file, int oflag, int mode);

/* Try to open a file using PREOPEN_FILES/PREOPEN_FILE_FDS (internal).
   Returns file descriptor on success, -1 on failure. */
extern int __preopen_file(const char *file, int oflag, int mode) attribute_hidden;
libc_hidden_proto(__preopen_file)

/* Try to open a file using PREOPEN_PATHS/PREOPEN_PATH_FDS with openat() (internal).
   Returns file descriptor on success, -1 on failure. */
extern int __preopen_from_dir(const char *file, int oflag, int mode) attribute_hidden;
libc_hidden_proto(__preopen_from_dir)

/* Get the debug output file descriptor from environment (internal).
   Returns FD on success, -1 if not configured. */
extern int __preopen_get_debug_fd(void);

/* Write debug log entry (internal).
   Arguments:
   - log_fd:   File descriptor for log output
   - prefix:   Log message prefix (e.g., "preopen", "open")
   - filepath: Path being operated on
   - fd:       Result file descriptor (positive=success, negative=failure)
*/
extern void __preopen_debug_log(int log_fd, const char *prefix, const char *filepath, int fd);

#endif /* _PREOPEN_H */
