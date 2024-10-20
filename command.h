#ifndef COMMAND_H
#define COMMAND_H

#define MAX_ARGS 100

typedef struct Command {
    char* args[MAX_ARGS];
    char* input_file;
    char* output_file;
    int append;
    struct Command *next;
} Command;

int parse_command(char* line, Command** cmd_head);
void free_commands(Command* cmd);

#endif
