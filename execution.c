/**
 * @author Liam Tang
 * @date October 15, 2024
 * @file execution.c
 * @brief Execute commands and manage processes for the shell.
 * 
 * This file contains functions to parse and execute shell commands, 
 * handle input/output redirection, pipes, and built-in commands. 
 * It also includes functions to manage foreground and background
 * processes, and to handle job control.
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "execution.h"
#include "command.h"
#include "signal_handling.h"
#include "job.h"
#include "error_handling.h"

#define MAX_PATH_LENGTH 1024
#define MAX_CMD_LENGTH 1000
#define MAX_ARGS 100

/**
 * @brief Parse and execute a command line.
 * @param line The input command line string.
 */
void execute_line(char* line){
    char line_copy[MAX_CMD_LENGTH];
    strncpy(line_copy, line, MAX_CMD_LENGTH);

    Command *cmd_head = NULL;

    //error generated during parsing command
    if(parse_command(line, &cmd_head) != 0){
        error_invalid_command();
        if(cmd_head != NULL) free_commands(cmd_head);
        return;
    }

    //empty command
    if(cmd_head == NULL){
        return;
    }

    //check if it is a build-in command, i.e. cd, jobs, fg or exit
    if(is_builtin(cmd_head)){
        if(cmd_head->next != NULL || cmd_head->input_file != NULL || cmd_head->output_file != NULL){
            error_invalid_command();
        }else{
            execute_builtin(cmd_head);
        }
        free_commands(cmd_head);
        return;
    }

    //execute the command if it is not a build-in one
    execute_commands(cmd_head, line_copy);
    free_commands(cmd_head);
}

/**
 * @brief Check if a command is a built-in command: `cd`, `exit`, `jobs`, or `fg`.
 * @param cmd The command structure to check.
 * @return 1 if the command is built-in, 0 otherwise.
 */
int is_builtin(Command* cmd){
    char* cmdname = cmd->args[0];
    return strcmp(cmdname, "cd") == 0 || strcmp(cmdname, "exit") == 0 
           || strcmp(cmdname, "jobs") == 0 || strcmp(cmdname, "fg") == 0;
}

/**
 * @brief Execute built-in shell commands.
 * @param cmd The command structure representing the built-in command.
 */
void execute_builtin(Command* cmd){
    char* cmdname = cmd->args[0];

    if(strcmp(cmdname, "cd") == 0){
        int arg_count = 0;
        while(cmd->args[arg_count] != NULL) arg_count++;
        //either 0 or 2+ arguments are invalid 
        if(arg_count != 2){
            error_invalid_command();
            return;
        }
        //change the directory according to the provided path
        if(chdir(cmd->args[1]) != 0){
            error_invalid_directory();
        }
    }else if(strcmp(cmdname, "exit") == 0){
        int arg_count = 0;
        while(cmd->args[arg_count] != NULL) arg_count++;
        //no other arguments should appear after 'exit'
        if(arg_count != 1){
            error_invalid_command();
            return;
        }
        //do not exit the shell if there is non-terminanted jobs
        if(job_count > 0){
            error_suspended_jobs();
        }else{
            exit(0);
        }
    }else if(strcmp(cmdname, "jobs") == 0){
        int arg_count = 0;
        while(cmd->args[arg_count] != NULL) arg_count++;
        //no other arguments should appear after 'jobs'
        if(arg_count != 1){
            error_invalid_command();
            return;
        }
        //show all stopped jobs
        print_jobs();
    }else if(strcmp(cmdname, "fg") == 0){
        int arg_count = 0;
        while(cmd->args[arg_count] != NULL) arg_count++;
        //arguments should only include 'fg' and an int index
        if(arg_count != 2){
            error_invalid_command();
            return;
        }
        int index = atoi(cmd->args[1]);
        //if the following argument is not an int or index out of bound, error
        if(index < 1 || index > job_count){
            error_invalid_job();
            return;
        }
        pid_t pid = jobs[index - 1].pid;
        char cmdline[MAX_CMD_LENGTH];
        strncpy(cmdline, jobs[index - 1].cmdline, MAX_CMD_LENGTH);
        //reomove the chosen job from the stopped list and continue the process
        remove_job(pid);
        if(kill(pid, SIGCONT) < 0){
            perror("kill");
        }
        //reap the finished process
        int status;
        waitpid(pid, &status, WUNTRACED);
        //if process is suspended, add it back to the jobs lsit
        if(WIFSTOPPED(status)){
            add_job(pid, cmdline);
        }
    }
}

/**
 * @brief Execute a sequence of commands with optional redirection and pipes.
 * @param cmd_head The head of the linked list of commands to execute.
 * @param cmdline The original command line string.
 */
void execute_commands(Command* cmd_head, char* cmdline){
    Command* cmd = cmd_head;
    int input_fd = -1;
    int fd[2];
    int prev_fd = -1;
    pid_t pid;
    int status;

    //create an array to store pids for the pipeline
    pid_t pids[MAX_ARGS];
    int pid_count = 0;

    while(cmd != NULL){
        //handle input redirection
        if(cmd->input_file != NULL){
            input_fd = open(cmd->input_file, O_RDONLY);
            if(input_fd < 0){
                error_invalid_file();
                //close any previous pipes
                if(prev_fd != -1) close(prev_fd);
                return;
            }
        }

        //create pipe if not the last command
        if(cmd->next != NULL){
            if(pipe(fd) < 0){
                perror("pipe");
                return;
            }
        }

        pid = fork();
        if(pid < 0){
            perror("fork");
            return;
        }else if (pid == 0){
            //restore default signal handlers
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            //handle input redirection
            if(cmd->input_file != NULL){
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
            }else if(prev_fd != -1){
                dup2(prev_fd, STDIN_FILENO);
                close(prev_fd);
            }

            //handle output redirection or write to pipe
            if(cmd->next == NULL){
                if(cmd->output_file != NULL){
                    int flags = O_WRONLY | O_CREAT;
                    if(cmd->append){
                        flags |= O_APPEND;//append to the file
                    }else{
                        flags |= O_TRUNC; //overwriet the file
                    }
                    int output_fd = open(cmd->output_file, flags, 0777); 
                    if(output_fd < 0){
                        error_invalid_file();
                        exit(EXIT_FAILURE);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                }
            }else{
                dup2(fd[1], STDOUT_FILENO); //redirect output to pipe
                close(fd[0]);
                close(fd[1]);
            }

            //close unused file descriptors to avoid leaks
            if(prev_fd != -1) close(prev_fd);

            //locate the program, either in '/usr/bin' or '/bin'
            char* search_paths[] = {"/usr/bin", "/bin", NULL};
            char program_path[MAX_PATH_LENGTH];
            int found = 0;

            if(cmd->args[0][0] == '/'){
                //start with '/', so absolute path
                strcpy(program_path, cmd->args[0]);
                found = 1;
            }else if(strchr(cmd->args[0], '/')){
                //contains '/', so relative path
                strcpy(program_path, cmd->args[0]);
                found = 1;
            }else{
                //search in /usr/bin and /bin
                for(int i = 0; search_paths[i] != NULL; i++){
                    //build program path
                    snprintf(program_path, sizeof(program_path), "%s/%s", search_paths[i], cmd->args[0]);
                    if(access(program_path, X_OK) == 0){
                        found = 1;
                        break;
                    }
                }
            }

            if(!found){
                error_invalid_program();
                exit(EXIT_FAILURE);
            }

            execv(program_path, cmd->args);
            error_invalid_program();
            exit(EXIT_FAILURE);
        } else {
            //parent process
            pids[pid_count++] = pid;

            if(prev_fd != -1) close(prev_fd);
            if(input_fd != -1) close(input_fd);
            if(cmd->next != NULL) {
                close(fd[1]);
                prev_fd = fd[0];
            }
        }
        cmd = cmd->next;
    }

    //wait for all child processes
    for(int i = 0; i < pid_count; i++){
        waitpid(pids[i], &status, WUNTRACED);
        if(WIFSTOPPED(status)) add_job(pids[i], cmdline);
    }
}
