#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ENV_VALUE_LENGTH 100

int main(int argc, char *argv[]) {
  const char *dir_path = "/tmp/preopen_test";
    
    // ディレクトリが存在しない場合は作成
    struct stat st = {0};
    if (stat(dir_path, &st) == -1) {
        if (mkdir(dir_path, 0700) == -1) {
            perror("Error creating directory");
            return 1;
        }
        printf("Created directory: %s\n", dir_path);
    }

    // ディレクトリをオープン
    int fd = open(dir_path, O_RDONLY | O_DIRECTORY);
    if (fd == -1) {
        perror("Error opening directory");
        return 1;
    }
    printf("Opened directory. File descriptor: %d\n", fd);

    // FDを文字列に変換
    char fd_str[16];
    snprintf(fd_str, sizeof(fd_str), "%d", fd);

    // 既存のPREOPEN_FDS環境変数を取得
    char *existing_fds = getenv("PREOPEN_FDS");
    char new_env_value[MAX_ENV_VALUE_LENGTH];

    if (existing_fds) {
        // 既存の値に新しいFDを追加
        snprintf(new_env_value, sizeof(new_env_value), "%s:%s", existing_fds, fd_str);
    } else {
        // 新しい値を設定
        strncpy(new_env_value, fd_str, sizeof(new_env_value) - 1);
        new_env_value[sizeof(new_env_value) - 1] = '\0';
    }

    // 環境変数を設定
    if (setenv("PREOPEN_FDS", new_env_value, 1) != 0) {
        perror("Error setting environment variable");
        close(fd);
        return 1;
    }

    printf("Set PREOPEN_FDS environment variable to: %s\n", new_env_value);
  
		if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    const char *filename = argv[1];
		printf("filename: %s\n", filename);
    fd = open(filename, O_RDONLY);
    
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    printf("%d\n", fd);
    
    // ファイルディスクリプタを閉じずにプログラムを終了
    // 親プロセスでこのFDを使用することを想定
    
    return 0;
}
