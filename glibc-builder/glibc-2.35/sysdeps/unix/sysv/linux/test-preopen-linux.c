/* Test that preopen functionality works correctly in open64.
	 Linux version.

	 Copyright (C) 2024 Free Software Foundation, Inc.
	 This file is part of the GNU C Library.

	 The GNU C Library is free software; you can redistribute it and/or
	 modify it under the terms of the GNU Lesser General Public
	 License as published by the Free Software Foundation; either
	 version 2.1 of the License, or (at your option) any later version.
	 */

#include <array_length.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Return true if FD is contained in the array [FDS, FDS + COUNT].  */
	static bool
check_fd_in_list (int fd, int *fds, size_t count)
{
	for (size_t i = 0; i < count; ++i)
		if (fds[i] == fd)
			return true;
	return false;
}

#define test_preopen(experr, path, flags)                        \
	(__extension__ ({                                             \
									errno = 0xdead;                                            \
									int expected_fd[] = experr;                                \
									int ret = preopen (path, flags);                           \
									int err = errno;                                           \
									int fail;                                                  \
									if (check_fd_in_list (ret, expected_fd, array_length (expected_fd))) \
									fail = 0;                                                \
									else                                                       \
									{                                                        \
									fail = 1;                                             \
									printf ("FAIL: preopen: path=%s expected one of %s, got fd=%d\n", \
											path, #experr, ret);                           \
											}                                                        \
											fail;                                                      \
											}))

	static int
do_test (void)
{
	int fails = 0;

	// 環境変数の設定
	setenv("PREOPEN_FDS", "3:4:5", 1);
	setenv("PREOPEN_PATHS", "/usr/local:/tmp:/home", 1);

	// テストケース1: マッチするパス
	fails |= test_preopen (LIST(3), "/usr/local/test.txt", O_RDONLY);

	// テストケース2: マッチしないパス
	fails |= test_preopen (LIST(-1), "/var/log/test.txt", O_RDONLY);

	// テストケース3: マッチする別のパス
	fails |= test_preopen (LIST(4), "/tmp/test.txt", O_RDONLY);

	// テストケース4: 相対パス（マッチしない）
	fails |= test_preopen (LIST(-1), "local/test.txt", O_RDONLY);

	return fails;
}

#include "support/test-driver.c"
