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
#include <shlib-compat.h>
#include <stdio.h>

#define MAX_LIST_LEN   128
#define MAX_PATH_LEN   256
/* Open FILE with access OFLAG.  If O_CREAT or O_TMPFILE is in OFLAG,
   a third argument is the file protection.  */

// for runcap
__attribute__((unused))
static void debug_print(const char *message) {
	size_t len = strlen(message);
	INLINE_SYSCALL_CALL (write, 2, message, len);
}

__attribute__((unused))
static void debug_println(const char *message) {
	size_t len = strlen(message);
	INLINE_SYSCALL_CALL (write, 2, message, len);
	INLINE_SYSCALL_CALL (write, 2, "\n", 2);
}

__attribute__((unused))
static void debug_printf(const char *format, ...) {
	char buffer[1024];  // 適切なサイズを設定
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	size_t len = strlen(buffer);
	INLINE_SYSCALL_CALL(write, 2, buffer, len);
}

#ifdef PREOPEN_DEBUG
#define DEBUG_PRINT(msg) debug_print(msg)
#define DEBUG_PRINTLN(msg) debug_println(msg)
#define DEBUG_PRINTF(msg, ...) debug_printf(msg, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(msg) ((void)0)
#define DEBUG_PRINTLN(msg) ((void)0)
#define DEBUG_PRINTF(msg, ...) ((void)0)
#endif 

// for runcap
// PREOPEN_FDSやPREOPEN_PATHSなど、環境変数で指定されたカンマ区切りの文字列を、配列に変換する関数
// str : 環境変数で取得した文字列
static char** get_string_list(char* env_str){
	int cnt = 0;

	char** result_list = malloc(sizeof(char *) * MAX_LIST_LEN);
	if (result_list == NULL){
		return NULL;
	}
	
	DEBUG_PRINT("begin get_string_list\n");
	// preopen_fdsに指定されたFDを：で区切って、intに変換した後にリストに入れる
	if (env_str){
		char *token = strtok(env_str, ":");
		while (token && cnt < MAX_LIST_LEN) {
			char *buff = token;
			result_list[cnt] = strdup(buff);
			if (result_list[cnt] == NULL) {
				// メモリ確保失敗時の処理を追加
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

 DEBUG_PRINT("end get_string_list\n");
	// NULL終端にしてリストを返す
	result_list[cnt] = NULL;
	return result_list;
}

// n番目のディレクトリ部分を取得する関数
// 戻り値: 成功時は取得した文字列へのポインタ、失敗時はNULL
// 引数:
//   path: 解析するパス
//   n: 取得したい位置（0から開始）
//   result: 結果を格納するバッファ
//   result_size: バッファのサイズ
static char* get_path_component(const char *path, int n, char *result, size_t result_size) {
	DEBUG_PRINTLN("----- begin get_path_component");
	if (path == NULL || result == NULL || result_size == 0) {
		return NULL;
	}
	
	const char *start = path + 1;  // 最初の'/'をスキップ
	const char *end;
	int current = 0;

	while (current < n) {
		start = strchr(start, '/');
		if (start == NULL) {
			return NULL;  // n番目のコンポーネントが存在しない
		}
		start++;  // '/'をスキップ
		current++;
	}

	end = strchr(start, '/');
	if (end == NULL) {
		// 最後のコンポーネント
		if (strlen(start) >= result_size) {
			return NULL;  // バッファサイズ不足
		}
		strncpy(result, start, result_size - 1);
		result[result_size - 1] = '\0';
	} else {
		size_t len = end - start;
		if (len >= result_size) {
			return NULL;  // バッファサイズ不足
		}
		strncpy(result, start, len);
		result[len] = '\0';
	}

	DEBUG_PRINTLN("----- end get_path_component");
	return result;
}

// パスの比較を行う関数
// 戻り値: 一致するプリオープンパスのインデックス。見つからない場合は-1
// 引数
// file_path: 開きたいファイルのパス
// preopen_paths: PREOPEN_PATHSで指定されたパスのリスト
// path_count
int find_matching_preopen_path(const char *file_path, char **preopen_paths, int path_count) {
	DEBUG_PRINTLN("---- begin find_matching_preopen_path");
	char file_component[MAX_PATH_LEN];
	char preopen_component[MAX_PATH_LEN];

	for (int i = 0; i < path_count; i++) {
		int depth = 0;
		bool match = true;

		// パスの各コンポーネントを比較
		while (1) {
			// ファイルパスのコンポーネントを取得
			if (get_path_component(file_path, depth, file_component, MAX_PATH_LEN) == NULL) {
				break;
			}
			DEBUG_PRINTF("     file path component: %s\n", file_component);

			// プリオープンパスのコンポーネントを取得
			if (get_path_component(preopen_paths[i], depth, preopen_component, MAX_PATH_LEN) == NULL) {
				break;
			}
			DEBUG_PRINTF("     preopen path component: %s\n", preopen_component);

			// コンポーネントを比較
			if (strcmp(file_component, preopen_component) != 0) {
				match = false;
				break;
			}

			depth++;
		}

		if (match) {
			return i;
		}
	}

	DEBUG_PRINTLN("---- end find_matching_preopen_path");
	return -1;
}

// for runcap
// preopen機構の実態
// PREOPEN_FDS　 コンマ区切りのFDを取得
// PREOPEN_PATHS コンマ区切りのパスを取得
// FDSとPATHSは順番が一緒であることを前提にしている
//
// file:       open関数自体の引数であるファイルのパス
// oflag:      open関数に自体の引数にわたってくるファイルOpen時のFlag
// mode:       oflagでO_CREATE(ファイルの新規作成)が指定されていたときにつけるファイルのパーミッション
int preopen(const char *file, int oflag, int mode){
#ifdef PREOPEN_DEBUG
	debug_println("LIBC_DEBUG");
#endif

	DEBUG_PRINTLN("---- begin preopen");
	// for runcap
	// preopen_fdsの処理
	int preopen_fds[MAX_LIST_LEN];
	char** preopen_fds_list;
	int cnt = 0;
	
	// PREOPEN_FDSを取得し、NULLであれば通常のOpen関数の処理を実行
	char *preopen_fds_str = getenv("PREOPEN_FDS");
	if (preopen_fds_str != NULL){
		
		DEBUG_PRINTLN(preopen_fds_str);
		// preopen_fds_str がからの場合は処理が正常にできないのでreturn

		preopen_fds_list = get_string_list(preopen_fds_str);
		// fds_listに入っているfdはstringなので、キャストの処理をする
		while (*preopen_fds_list != NULL) {
			char *endptr;
			long int fd = strtol(*preopen_fds_list, &endptr, 10);
			
			if (*endptr != '\0') fd = -1;

			preopen_fds[cnt++] = (int)fd;
			preopen_fds_list++;
		}
	}
	else {
		DEBUG_PRINTLN("PREOPEN_FDS is NULL");
		return -1;
	}

	// preopen_pathの処理
	// preopen_pathはpreopen_fdsのFDに対応するパス（文字列）
	char **preopen_paths = NULL;
	char *preopen_paths_str = getenv("PREOPEN_PATHS");
	if (preopen_paths_str != NULL){
		DEBUG_PRINT("PREOPEN_PATHS=");
		DEBUG_PRINTLN(preopen_paths_str);
		// preopen_paths_str が空の場合は処理が正常にできないのでreturn
		preopen_paths = get_string_list(preopen_paths_str);
		DEBUG_PRINTF("preopen_paths 0: %s\n", preopen_paths[0]);
	}
	else {
		DEBUG_PRINTLN("PREOPEN_PATHS env is NULL\n");
		return -1;
	}

	// パスのマッチング
	// 絶対パスの場合の処理
	if (file[0] == '/') {
		int path_index = find_matching_preopen_path(file, preopen_paths, cnt);
		if (path_index >= 0) {
			DEBUG_PRINTF("file match. file:%s, preopen_dir:%s, fd:%d\n", file, preopen_paths[path_index], preopen_fds[path_index]);
			// マッチするパスが見つかった場合
			int fd;
			if (oflag & O_CREAT) {
				fd = SYSCALL_CANCEL(openat, preopen_fds[path_index], file, oflag, mode);
			} else {
				fd = SYSCALL_CANCEL(openat, preopen_fds[path_index], file, oflag);
			}
			// 該当するファイルが見つかり、Openできたとき
			if (fd != -1) {
				DEBUG_PRINTLN("abs preopen successful");
				return fd;
			}
			return -1;  // マッチするパスが見つからないか、openatが失敗した場合
		} else DEBUG_PRINTLN("not path index"); 
	} else {
	// パスが入っているものは、preopen_pathsからOpenを試みる
	// この処理はまだ未実装。必要かどうかも含めて考える必要あり
	//
	// 相対パスの場合、preopen_fdsからのopenを試みる
		int i, fd;
		for (i = 0; i < cnt; i++){
			if (oflag & O_CREAT) {
				fd = SYSCALL_CANCEL (openat, preopen_fds[i], file, oflag, mode);
			} else {
				fd = SYSCALL_CANCEL (openat, preopen_fds[i], file, oflag);
			}
			DEBUG_PRINTLN("try preopen\n");
			if (fd != -1) {
				DEBUG_PRINTLN("rel preopen successful");
				return fd;
			}
		}
		DEBUG_PRINTLN("rel preopen failed");
		return -1;
	}
	return -1;
}

	int
__libc_open64 (const char *file, int oflag, ...)
{
	int mode = 0;
	int fd = -1;

	DEBUG_PRINT("call open64_prepopen: ");
	DEBUG_PRINTLN(file);
	if (__OPEN_NEEDS_MODE (oflag))
	{
		va_list arg;
		va_start (arg, oflag);
		mode = va_arg (arg, int);
		va_end (arg);
	}
	
	fd = preopen(file, oflag, mode);
	if (fd > 0) {
		DEBUG_PRINTLN("preopen success");
		return fd;
	} else DEBUG_PRINTLN("preopen failed");
	DEBUG_PRINTLN("--- end preopen");

	DEBUG_PRINTF("call open: %s\n", file);
	fd = SYSCALL_CANCEL(openat, AT_FDCWD, file, oflag | O_LARGEFILE, mode);
	if (fd > 0) DEBUG_PRINTLN("open success");
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
