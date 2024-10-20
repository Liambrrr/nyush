#ifndef SIGNAL_HANDLING_H
#define SIGNAL_HANDLING_H

void handle_all_signals();
void sigint_handler(int sig);
void sigtstp_handler(int sig);

#endif