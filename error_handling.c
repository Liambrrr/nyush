/**
 * @author Liam Tang
 * @date October 12, 2024
 * @file error_handling.c
 * @brief handle specific errors.
 * 
 * This file contains functions that handle specific errors for the shell,
 * according to the project requirements. The errors handled including
 * invalid command, program, file, directory, job and suspended jobs are
 * reported to 'stderr' with appropriate error messages.
 * 
 * */

#include <stdio.h>
#include <unistd.h>
#include <libgen.h>
#include "error_handling.h"

#define MAX_PATH_LENGTH 1024

/**
 * @brief Prints an "invalid command" error message.
 */
void error_invalid_command(){
    fprintf(stderr, "Error: invalid command\n");
}

/**
 * @brief Prints an "invalid program" error message.
 */
void error_invalid_program(){
    fprintf(stderr, "Error: invalid program\n");
}

/**
 * @brief Prints an "invalid file" error message.
 */
void error_invalid_file(){
    fprintf(stderr, "Error: invalid file\n");
}

/**
 * @brief Prints an "invalid directory" error message.
 */
void error_invalid_directory(){
    fprintf(stderr, "Error: invalid directory\n");
}

/**
 * @brief Prints an "invalid job" error message.
 */
void error_invalid_job(){
    fprintf(stderr, "Error: invalid job\n");
}

/**
 * @brief Prints an error message for suspended jobs.
 */
void error_suspended_jobs(){
    fprintf(stderr, "Error: there are suspended jobs\n");
}
