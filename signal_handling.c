/**
 * @author Liam Tang
 * @date October 12, 2024
 * @file signal_handling.c
 * @brief handle specific signals.
 * 
 * This file contains functions that handle specific signals for the shell,
 * according to the project requirements. The signals handled include
 * SIGINT (Ctrl-C), SIGQUIT (Ctrl-\), and SIGTSTP (Ctrl-Z), which are
 * ignored to prevent the shell from terminating or suspending itself.
 * 
 * */

#include <signal.h>
#include "signal_handling.h"

/**
 * @brief Handle SIGINT (Ctrl-C) and SIGQUIT (Ctrl-\) signals.
 * @param sig The signal number.
 */
void sigint_handler(int sig){
    //ignore SIGINT caused by Ctrl-C and SIGQUIT caused by Ctrl-\, so nothing happens here
}

/**
 * @brief Handle SIGTSTP (Ctrl-Z) signal.
 * @param sig The signal number.
 */
void sigtstp_handler(int sig){
    //ignore SIGTSTP caused by Ctrl-Z, so nothing happens here
}

/**
 * @brief Set up handlers to ignore specific signals.
 */
void handle_all_signals(){
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
}
