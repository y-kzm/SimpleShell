/**
 * @file kzmsh.c
 * @author Kazuma Yokoo
 * @brief 超シンプルな自作シェル
 * @version 0.1
 * @date 2023-05-08
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFSIZE 64

static int buitin_help();
static int builtin_exit();
static int builtin_cd();

static void shell_loop();
static char **analys_cmd(char *line);
static int execute_cmd(char **args);

/**
 * @brief ヘルプを表示する
 * 
 * @return int 
 */
static int buitin_help()
{
    printf("Simple Shell\n");
    printf("Exit with \"exit\"\n");
    printf("Warning: buffer overflow\n");
    return 1;
}

/**
 * @brief exitを実行する
 * 
 * @return int 
 */
static int builtin_exit()
{
    printf("Good Bye...\n");
    return 0;
}

/**
 * @brief cdコマンドを実行する（未サポート）
 * 
 * @return int 
 */
static int builtin_cd()
{
    printf("cd is not supported yet\n");
    return 1;
}

/**
 * @brief シェルのメインループ処理
 * 
 */
static void shell_loop()
{
    char *line = NULL; 
    size_t len = 0;
    ssize_t nread;
    char **args;
    int status;

    // SIGINTを無視
    signal(SIGINT, SIG_IGN);

    do {
		printf("\x1b[32m> \x1b[39m");
        // 標準入力から1行読み取る
        if ( (nread = getline(&line, &len, stdin)) == -1 ) {
            printf("Failed to read line\n");
            exit(EXIT_FAILURE);
        }
        
        // コマンドを解析・実行する
        args = analys_cmd(line);
        status = execute_cmd(args);

        free(args);
    } while(status); 
    
    free(line);
}

/**
 * @brief コマンドを解析する
 * 
 * @param line 
 * @return char** 
 */
static char **analys_cmd(char *line)
{
    char **cmds = malloc(BUFSIZE * sizeof(char*));
    char *cmd;
    int position = 0;

    // コマンドをスペースと改行で分解する
    cmd = strtok(line, " \n");
	while (cmd != NULL) {
        // ポインタ配列に区切り文字のポインタを格納
		cmds[position] = cmd;
        position++;
        cmd = strtok(NULL, " \n");
    }

    return cmds;
}

/**
 * @brief コマンドを実行する
 * 
 * @param args 
 * @return int 
 */
static int execute_cmd(char **args)
{
    pid_t pid;
    int status;

    // コマンド入力がない場合は無視
    if (args[0] == NULL) {
        return 1;
    }

    // ビルトインコマンドが入力された場合の処理
    if (strcmp(args[0], "exit") == 0) {
        return builtin_exit();
    } else if (strcmp(args[0], "help") == 0) {
        return buitin_help();
    } else if (strcmp(args[0], "cd") == 0) {
        return builtin_cd();
    }

    pid = fork();
    if (pid < 0) {
        printf("Error: can't fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        /* 子プロセス */
        // PATH環境変数からコマンドを実行
        if ( (execvp(args[0], args)) == -1 ) {
            printf("command not found: %s\n", args[0]);
            _exit(EXIT_FAILURE);
        } else {
            _exit(EXIT_SUCCESS);
        }
    } else {
        /* 親プロセス */
        wait(&status);
    }

    return 1;
}

/**
 * @brief エントリーポイント
 * 
 * @return int 
 */
int main()
{
    shell_loop();

    return 0;
}