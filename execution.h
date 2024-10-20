#ifndef EXECUTION_H
#define EXECUTION_H

#include "command.h"

void execute_line(char* line);
void execute_builtin(Command* cmd);
void execute_commands(Command* cmd_head, char* cmdline);
int is_builtin(Command* cmd);

#endif
