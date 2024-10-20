/**
 * @author Liam Tang
 * @date October 12, 2024
 * @file command.c
 * @brief parse input lines into command structures.
 * 
 * This file contains functions that parse an input line into an actual
 * commmand, containing attributes of an array of char pointers (args),
 * two char pointers (input_file, output_file) and an int (append).
 * After execution, the command would be free by calling free_commands()
 * in this file.
 * 
 * */

#include <string.h>
#include <stdlib.h>
#include "command.h"
#include "error_handling.h"

/**
 * @brief Parse an input line into a linked list of Command structures.
 * @param line The input command line string.
 * @param cmd_head Pointer to the head of the `Command` linked list.
 * @return 0 if parsing is successful, or -1 if there is a error.
 */
int parse_command(char* line, Command** cmd_head){
    char* part;
    Command* current_cmd = NULL;
    int input_count = 0;
    int output_count = 0;
    int has_pipe = 0;

    //start parsing the line into a command part by part
    part = strtok(line, " ");
    while(part != NULL){
        if(strcmp(part, "|") == 0){
            //invalid command since '|' must follow a valid command
            if(current_cmd == NULL || current_cmd->args[0] == NULL){
                return -1;
            }

            //get ready for the command after pipe
            current_cmd->next = (Command *)calloc(1, sizeof(Command));
            current_cmd = current_cmd->next;
            has_pipe = 1;
        }else if(strcmp(part, "<") == 0){
            part = strtok(NULL, " ");
            //invalid command since '<' must follow a valid command and must have an input file following
            if(part == NULL || input_count > 0 || (has_pipe && current_cmd != *cmd_head)){
                return -1;
            }

            current_cmd->input_file = part;
            input_count++;
        }else if(strcmp(part, ">") == 0 || strcmp(part, ">>") == 0){
            int is_append = (strcmp(part, ">>") == 0);
            part = strtok(NULL, " ");
            //invalid command since '>' and '>>' must follow a valid command and must have an output file following
            if(part == NULL || output_count > 0 || (has_pipe && current_cmd->next != NULL)){
                return -1;
            }
            current_cmd->output_file = part;
            current_cmd->append = is_append;
            output_count++;
        }else{
            //if no command exists, create a new one
            if(current_cmd == NULL) {
                current_cmd = (Command* )calloc(1, sizeof(Command));
                *cmd_head = current_cmd;
            }
            int arg_pos = 0;
            while (current_cmd->args[arg_pos] != NULL) arg_pos++;
            current_cmd->args[arg_pos] = part;
            current_cmd->args[arg_pos + 1] = NULL;
        }
        part = strtok(NULL, " ");
    }

    //invalid empty command 
    if(current_cmd == NULL || current_cmd->args[0] == NULL){
        return -1;
    }

    //invalid command due to several input/output files
    if(input_count > 1 || output_count > 1){
        return -1;
    }

    return 0;
}

/**
 * @brief Free the memory allocated for command structures.
 * @param cmd A pointer to the head of the `Command` linked list.
 */
void free_commands(Command* cmd){
    while (cmd != NULL) {
        Command* tmp = cmd;
        cmd = cmd->next;
        free(tmp);
    }
}
