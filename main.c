/**
 * @author Liam Tang
 * @date October 17, 2024
 * @file main.c
 * @brief Entry point for the shell program.
 *
 * This file serves as the main entry point for the custom shell ("nyush").
 * It includes the `main()` function that controls the shell's primary loop
 * and input handling, and the `print_prompt()` function, which prints the 
 * shell's prompt with the current directory.
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/wait.h>
#include "signal_handling.h"
#include "error_handling.h"
#include "command.h"
#include "execution.h"
#include "job.h"

#define MAX_PATH_LENGTH 1024
#define MAX_CMD_LENGTH 1000

/**
 * @brief Prints the shell prompt with the current directory.
 */
void print_prompt(){
    char cwd[MAX_PATH_LENGTH];
    getcwd(cwd, sizeof(cwd));
    char* base_name = basename(cwd);
    printf("[nyush %s]$ ", base_name);
    fflush(stdout);
}

/**
 * @brief The main function that runs the shell.
 */
int main(){
    handle_all_signals();
    char line[MAX_CMD_LENGTH];

    //keep running the shell
    while(1){
        print_prompt();
        if(fgets(line, MAX_CMD_LENGTH, stdin) == NULL){
            if(job_count > 0){
                error_suspended_jobs();
                continue;
            }else{
                break;
            }
        }
        size_t line_len = strlen(line);
        if(line_len > 0 && line[line_len - 1] == '\n'){
            line[line_len - 1] = '\0';
        }

        //ignore empty command line
        if(line[0] == '\0'){
            continue;
        }

        execute_line(line);

        //reap any background processes
        while(waitpid(-1, NULL, WNOHANG) > 0){}
    }
    return 0;
}