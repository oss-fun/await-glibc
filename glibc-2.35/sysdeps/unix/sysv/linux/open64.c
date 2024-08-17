/* Linux open syscall implementation, LFS.
   Copyright (C) 1991-2022 Free Software Foundation, Inc.
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
#include <sysdep-cancel.h>
//#include <stdio.h>
#include <shlib-compat.h>

#define MAX_PREOPEN_DIRS 128
/* Open FILE with access OFLAG.  If O_CREAT or O_TMPFILE is in OFLAG,
   a third argument is the file protection.  */
int
__libc_open64 (const char *file, int oflag, ...)
{
  int mode = 0;

	INLINE_SYSCALL_CALL (write, 2, "open64_preopen\n", 16);
  if (__OPEN_NEEDS_MODE (oflag))
    {
      va_list arg;
      va_start (arg, oflag);
      mode = va_arg (arg, int);
      va_end (arg);
    }

	// for runcap
	int preopen_dirs[MAX_PREOPEN_DIRS];
	int cnt = 0;
	char *preopen_fds = getenv("PREOPEN_FDS");
	if (preopen_fds){
		char *token = strtok(preopen_fds, ":");
		while (token && cnt < MAX_PREOPEN_DIRS) {
			INLINE_SYSCALL_CALL(write, "token\n", 6);
			cnt++;
			char *endptr;
			char *buff = token;
			long int fd = strtol(buff, &endptr, 10);
			
			if (*endptr != '\0' || fd < 0 || fd > INT_MAX) {
				continue;
			}
			else preopen_dirs[cnt] = (int)fd;

			token = strtok(NULL, ":");
		}
	}

	int last_errno = 0;
	int i, fd;
	for (i = 0; i < cnt; i++){
		fd = SYSCALL_CANCEL (openat, preopen_dirs[i], file, oflag | O_LARGEFILE, mode);
		INLINE_SYSCALL_CALL(write, 2, "try preopen\n", 11);
		last_errno = errno;
		if (fd != -1) return fd;

	}
	fd = SYSCALL_CANCEL(open, file, oflag | O_LARGEFILE, mode);
	if (fd == -1 && last_errno != 0 ) errno = last_errno;

	return fd;
}

strong_alias (__libc_open64, __open64)
libc_hidden_weak (__open64)
weak_alias (__libc_open64, open64)

#ifdef __OFF_T_MATCHES_OFF64_T
strong_alias (__libc_open64, __libc_open)
strong_alias (__libc_open64, __open)
libc_hidden_weak (__open)
weak_alias (__libc_open64, open)
#endif

#if OTHER_SHLIB_COMPAT (libpthread, GLIBC_2_1, GLIBC_2_2)
compat_symbol (libc, __libc_open64, open64, GLIBC_2_2);
#endif
