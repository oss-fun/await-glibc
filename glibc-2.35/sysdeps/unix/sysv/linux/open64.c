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
#include <unistd.h>

#define MAX_LIST_LEN   128
#define MAX_PATH_LEN   256
/* Open FILE with access OFLAG.  If O_CREAT or O_TMPFILE is in OFLAG,
   a third argument is the file protection.  */

// for runcap
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
#define DEBUG_PRINTF(msg, ...) debug_printf(msg, ##__VA_ARGS__)
#else
#define DEBUG_PRINTF(msg, ...) ((void)0)
#endif

// DEBUGメッセージを出力するファイルを出力
// PREOPEN_DEBUG_FDが指定されているかを確認
// 指定されていなければ、PREOPEN_DEBUG_FILEを確認
// この環境変数で指定されたファイルをオープン
static inline int get_debug_fd(void) {
	// FDが環境変数で指定されているかチェック
	const char *fd_str = getenv("PREOPEN_DEBUG_FD");
	if (fd_str) {
		// 文字列からFDに変換
		char *endptr;
		long fd = strtol(fd_str, &endptr, 10);
		if (*endptr == '\0' && fd > 0) {
			return (int)fd;
		}
	}

	// FDが指定されていない場合は、パスからオープン
	const char *debug_log_path = getenv("PREOPEN_DEBUG_FILE");
	if (!debug_log_path) {
		return -1;
	}

	return SYSCALL_CANCEL(open, debug_log_path, O_WRONLY | O_APPEND | O_CREAT, 0666);
}

// DEBUG用ファイルにメッセージを出力
void debug_log(int log_fd, const char *prefix, const char *filepath, int fd) {
	if (log_fd <= 0) return;

	char buffer[4096] = {0};
	int len = 0;

	// prefix + ": "
	int prefix_len = strlen(prefix);
	memcpy(buffer + len, prefix, prefix_len);
	len += prefix_len;
	memcpy(buffer + len, ": ", 2);
	len += 2;

	// filepath
	int path_len = strlen(filepath);
	memcpy(buffer + len, filepath, path_len);
	len += path_len;

	// status (スペースを含む)
	if (fd > 0) {
		memcpy(buffer + len, " success", 8);
		len += 8;
	} else {
		memcpy(buffer + len, " failed", 7);
		len += 7;
	}

	// newline
	buffer[len] = '\n';
	len += 1;

	// single write
	INLINE_SYSCALL_CALL(write, log_fd, buffer, len);
}

// pathを入力すると、最後のコンポーネントを削除
// 例: /a/b/c.txt -> /a/b
char *remove_last_component(const char *path) {
	if (!path) return NULL;

	// 結果用のバッファを確保
	char *result = strdup(path);
	if (!result) return NULL;

	// 末尾のスラッシュを削除
	size_t len = strlen(result);
	while (len > 0 && result[len - 1] == '/') {
		result[--len] = '\0';
	}

	// 最後の'/'を探す
	char *last_slash = strrchr(result, '/');
	if (last_slash) {
		// 最後の'/'で切る
		*last_slash = '\0';
		// ルートディレクトリの場合は'/'を残す
		if (last_slash == result) {
			result[1] = '\0';
		}
	} else {
		// スラッシュが見つからない場合は空文字を返す
		result[0] = '\0';
	}

	return result;
}
char *get_absolute_path(const char *path) {
	char cwd[PATH_MAX];
	char *abs_path = malloc(PATH_MAX);

	if (!abs_path) return NULL;
	memset(abs_path, 0, PATH_MAX);

	// パスが既に絶対パスの場合
	if (path[0] == '/') {
		strncpy(abs_path, path, PATH_MAX - 1);
		abs_path[PATH_MAX - 1] = '\0';
		return abs_path;
	}

	// カレントディレクトリの取得
	if (getcwd(cwd, sizeof(cwd)) == NULL) {
		free(abs_path);
		return NULL;
	}

	// "./"で始まるパスの処理
	const char *path_to_append;
	if (strncmp(path, "./", 2) == 0) {
		path_to_append = path + 2;
	} else {
		path_to_append = path;
	}

	size_t cwd_len = strlen(cwd);
	size_t path_len = strlen(path_to_append);

	// 必要なバッファサイズをチェック
	if (cwd_len + path_len + 2 > PATH_MAX) {  // +2 は '/' と '\0' 用
		free(abs_path);
		return NULL;
	}

	// メモリコピーを使用して文字列を構築
	memcpy(abs_path, cwd, cwd_len);
	abs_path[cwd_len] = '/';
	memcpy(abs_path + cwd_len + 1, path_to_append, path_len);
	abs_path[cwd_len + path_len + 1] = '\0';

	// 末尾のスラッシュの処理
	size_t total_len = strlen(abs_path);
	if (total_len > 1 && abs_path[total_len - 1] == '/') {
		abs_path[total_len - 1] = '\0';
	}

	return abs_path;
}

// for runcap
// PREOPEN_FDSやPREOPEN_PATHSなど、環境変数で指定されたカンマ区切りの文字列を、配列に変換する関数
// str : 環境変数で取得した文字列
static char** get_string_list(char* env_str){
	int cnt = 0;

	char** result_list = malloc(sizeof(char *) * MAX_LIST_LEN);
	if (result_list == NULL){
		return NULL;
	}

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

	// NULL終端にしてリストを返す
	result_list[cnt] = NULL;
	return result_list;
}

// パスのn番目を取得する関数
// 戻り値: 成功時は取得した文字列へのポインタ、失敗時はNULL
// 引数:
//   path: 解析するパス
//   n: 取得したい位置（0から開始）
//   result: 結果を格納するバッファ
static char* get_path_component(const char *path, int n) {
	char *result = malloc(sizeof(char) * MAX_PATH_LEN);
	if (path == NULL ) return NULL;

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
		if (strlen(start) >= MAX_PATH_LEN) {
			return NULL;  // バッファサイズ不足
		}
		strncpy(result, start, MAX_PATH_LEN - 1);
		result[MAX_PATH_LEN - 1] = '\0';
	} else {
		size_t len = end - start;
		if (len >= MAX_PATH_LEN) {
			return NULL;  // バッファサイズ不足
		}
		strncpy(result, start, len);
		result[len] = '\0';
	}

	return result;
}

// パスの比較を行う関数
// 戻り値: 一致するプリオープンパスのインデックス。見つからない場合は-1
// 引数
// file_path: 開きたいファイルのパス
// preopen_paths: PREOPEN_PATHSで指定されたパスのリスト
// path_count
int find_matching_preopen_path(const char *file_path, char **preopen_paths, int path_count) {
	char *file_component;
	char *preopen_component;
	//int fd_depth[path_count];
	DEBUG_PRINTF("path_count: %d\n", path_count);

	for (int i = 0; i < path_count; i++) {
		int depth = 0;
		bool match = true;

		// パスの各コンポーネントを比較
		while (1) {
			// ファイルパスのコンポーネントを取得
			file_component = get_path_component(file_path, depth);
			// プレオープンパスのコンポーネントを取得
			preopen_component = get_path_component(preopen_paths[i], depth);
			DEBUG_PRINTF("index [%d] path_component: %s, preopen path component: %s\n", i, file_component, preopen_component);

			if (file_component == NULL && preopen_component == NULL) break;
			if ((file_component == NULL) != (preopen_component == NULL)) {
				match = false;
				break;
			}
			// コンポーネントを比較して、一致した場合falseにする
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

	return -1;
}

// PREOPEN_FILESの処理
// PREOPEN_FILESで指定されたファイルは、PREOPEN_FILE_FDSに対応する
int preopen_file(const char *file, int oflag, int mode){
	//PREOPEN_FILE用の変数
	DEBUG_PRINTF("in preopen_file\n");
	int cnt = 0;
	char *preopen_files_str;
	char *preopen_file_fds_str;
	char **preopen_files = NULL;
	char **preopen_file_fds_list = NULL;
	int preopen_file_fds[MAX_LIST_LEN];

	// PREOPEN_FILESをchar* のリストに変換
	char* buff = getenv("PREOPEN_FILES");
	if (buff != NULL){
		DEBUG_PRINTF("PREOPEN_FILES=%s\n", buff);
		preopen_files_str = strdup(buff);
		//コンマ区切りパスから、パスのリストを生成
		preopen_files = get_string_list(preopen_files_str);
		free(preopen_files_str);
	} else {
		DEBUG_PRINTF("PREOPEN_FILES is not set\n");
		return -1;
	}

	// PREOPEN_FILE_FDSをchar*のリストに変換したあと、intに変換
	buff = getenv("PREOPEN_FILE_FDS");
	if (buff != NULL){
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

	// パスのマッチング
	// 絶対パスの場合の処理
	const char *abs_file = file;
	if (file[0] != '/') abs_file = get_absolute_path(file);
	DEBUG_PRINTF("abs_file: %s\n", abs_file);
	int path_index = find_matching_preopen_path(abs_file, preopen_files, cnt);
	if (path_index >= 0) {
		DEBUG_PRINTF("preopen file match. file:%s, preopen file:%s, fd:%d\n", abs_file, preopen_files[path_index], preopen_file_fds[path_index]);
		// マッチするパスが見つかった場合
		int fd = preopen_file_fds[path_index];
		// 該当するファイルが見つかり、Openできたとき
		DEBUG_PRINTF("file preopen successful\n");
		return fd;
	}
	DEBUG_PRINTF("file no match\n"); 
	return -1;
}

int preopen_from_dir(const char *file, int oflag, int mode){
	//PREOPEN_PATHSの処理
	//PREOPEN_PATHS用の変数
	int cnt = 0;
	char *preopen_paths_str;
	char *preopen_path_fds_str;
	char **preopen_paths = NULL;
	char **preopen_path_fds_list = NULL;
	int preopen_path_fds[MAX_LIST_LEN];
	DEBUG_PRINTF("in preopen_from_dir\n");
	// PREOPEN_PATHSで指定されたパスは、PREOPEN_PATH_FDSのFDに対応する
	// preopenするファイルパスのパスを、PREOPEN_PATHで指定されたパスと比べる
	// 最も近い（パスが似ている）ものを特定し、対応するFDをPREOPEN_PATH_FDSから取り出す
	/// PREOPEN_FILESをchar* のリストに変換
	char *buff = getenv("PREOPEN_PATHS");
	if (buff != NULL){
		DEBUG_PRINTF("PREOPEN_PATHS=%s\n", buff);
		preopen_paths_str = strdup(buff);
		//コンマ区切りパスから、パスのリストを生成
		preopen_paths = get_string_list(preopen_paths_str);
		free(preopen_paths_str);
	} else {
		DEBUG_PRINTF("PREOPEN_PATHS is not set\n");
		return -1;
	}

	// PREOPEN_PATH_FDSをchar*のリストに変換したあと、intに変換
	buff = getenv("PREOPEN_PATH_FDS");
	if (buff != NULL){
		DEBUG_PRINTF("PREOPEN_PATH_FDS=%s\n", buff);
		preopen_path_fds_str = strdup(buff);
		preopen_path_fds_list = get_string_list(preopen_path_fds_str);
		free(preopen_path_fds_str);
	} else {
		DEBUG_PRINTF("PREOPEN_PATH_FDS is not set\n");
		return -1;
	}
	// preopen_path_fds_listに入っているfdはstringなので、キャストの処理をする
	while (*preopen_path_fds_list != NULL) {
		char *endptr;
		long int fd = strtol(*preopen_path_fds_list, &endptr, 10);

		if (*endptr != '\0') fd = -1;

		preopen_path_fds[cnt++] = (int)fd;
		preopen_path_fds_list++;
	}

	// パスのマッチング
	// 絶対パスの場合の処理
	const char *abs_path = file;
	// ファイルパスが絶対パスでない場合、絶対パスに変換
	if (file[0] != '/') abs_path = get_absolute_path(file);
	// Openするパスがディレクトリの場合、最後のコンポーネントを削除しない
	if (!(oflag & O_DIRECTORY)) abs_path = remove_last_component(abs_path);

	DEBUG_PRINTF("preopen abs_path: %s\n", abs_path);
	int path_index = find_matching_preopen_path(abs_path, preopen_paths, cnt);
	if (path_index >= 0) {
		DEBUG_PRINTF("file match. file:%s, preopen_dir:%s, fd:%d\n", abs_path, preopen_paths[path_index], preopen_path_fds[path_index]);
		// マッチするパスが見つかった場合
		int fd;
		if (oflag & O_CREAT) {
			fd = SYSCALL_CANCEL(openat, preopen_path_fds[path_index], abs_path, oflag, mode);
		} else {
			fd = SYSCALL_CANCEL(openat, preopen_path_fds[path_index], abs_path, oflag);
		}
		// 該当するファイルが見つかり、Openできたとき
		if (fd != -1) {
			DEBUG_PRINTF("abs preopen successful\n");
			return fd;
		}
	} else DEBUG_PRINTF("not path index\n"); 
	return -1;
}

// for runcap
// preopen機構の実態
// PREOPEN_PATHS コンマ区切りのパスを取得
// PREOPEN_PATH_FDS コンマ区切りのパスのFDを取得
// PREOPEN_FILES コンマ区切りのファイルパスを取得
// PREOPEN_FILE_FDS コンマ区切りのファイルパスのFDを 
//
// file:       open関数自体の引数であるファイルのパス
// oflag:      open関数に自体の引数にわたってくるファイルOpen時のFlag
// mode:       oflagでO_CREATE(ファイルの新規作成)が指定されていたときにつけるファイルのパーミッション
int preopen(const char *file, int oflag, int mode){
	DEBUG_PRINTF("---- begin preopen\n");
	DEBUG_PRINTF("try open: %s\n", file);
	// for runcap
	int preopen_fd;
	preopen_fd = preopen_file(file, oflag, mode);
	if (preopen_fd > 0) return preopen_fd;

	preopen_fd = preopen_from_dir(file, oflag, mode);
	if (preopen_fd > 0) {
		char *test = getenv("PREOPEN_PATH_FDS");
		DEBUG_PRINTF("PREOPEN_PATH_FDS:%s\n", test);

		return preopen_fd;
	}
	
	char *test = getenv("PREOPEN_PATH_FDS");
	DEBUG_PRINTF("PREOPEN_PATH_FDS:%s\n", test);

	DEBUG_PRINTF("---- end preopen\n");
	return -1;
}

	int
__libc_open64 (const char *file, int oflag, ...)
{
	int mode = 0;
	int fd = -1;

	DEBUG_PRINTF("\ncall open64_prepopen: %s\n", file);
	if (__OPEN_NEEDS_MODE (oflag))
	{
		va_list arg;
		va_start (arg, oflag);
		mode = va_arg (arg, int);
		va_end (arg);
	}
	int log_fd = get_debug_fd();

	fd = preopen(file, oflag, mode);
	debug_log(log_fd, "preopen", file, fd);
	if (fd > 0) return fd;

	fd = SYSCALL_CANCEL(openat, AT_FDCWD, file, oflag | O_LARGEFILE, mode);

	// DEBUG用の処理を追加
	debug_log(log_fd, "open", file, fd);
	INLINE_SYSCALL_CALL(close, log_fd);
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
