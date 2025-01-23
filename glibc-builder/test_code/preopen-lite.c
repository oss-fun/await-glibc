#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENV_VALUE_LENGTH 100

int main(int argc, char *argv[]) {
	char *path1 = "/tmp/preopen_test";
	char *path2 = "/tmp/preopen_test2";

	// ディレクトリをオープン
	int fd = open(path1, O_RDONLY | O_DIRECTORY);
	int fd2 = open(path2, O_RDONLY | O_DIRECTORY);

	// 環境変数を設定
	if (setenv("PREOPEN_FDS", "3:4", 1) != 0) {
		perror("Error setting environment variable");
		close(fd);
		return 1;
	}

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}

	const char *filename = argv[1];
	printf("filename: %s\n", filename);
	int preopen_fd = open(filename, O_RDONLY);

	if (fd == -1) {
		perror("Error opening file");
		exit(1);
	}

	printf("%d\n", preopen_fd);

	// ファイルディスクリプタを閉じずにプログラムを終了
	// 親プロセスでこのFDを使用することを想定

	return 0;
}
