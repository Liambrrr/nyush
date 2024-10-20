/**
 * @author Liam Tang
 * @date October 13, 2024
 * @file job.c
 * @brief Implementation of job management functions for the shell.
 * 
 * This file implements functions to manage background jobs in the shell. It
 * provides functionality to add a job, remove a job by its process ID, and print
 * the current list of jobs. 
 * 
 * The job information is stored in a global array of `Job` structures, and the
 * current count of jobs is tracked by a global `job_count` variable.
 */

#include <stdio.h>
#include <string.h>
#include "job.h"
#include "error_handling.h"

/**
 * @brief Array to store jobs, with a maximum of MAX_JOBS.
 */
Job jobs[MAX_JOBS];


/**
 * @brief Global counter to track the number of jobs in the array.
 */
int job_count = 0;

/**
 * @brief Add a new job to the list of jobs.
 * @param pid The process ID of the job to add.
 * @param cmdline The command line string that started the job.
 */
void add_job(pid_t pid, char* cmdline){
    if(job_count < MAX_JOBS){
        jobs[job_count].pid = pid;
        strncpy(jobs[job_count].cmdline, cmdline, MAX_CMD_LENGTH);
        job_count++;
    }
}

/**
 * @brief Remove a job from the list of jobs by process ID.
 * @param pid The process ID of the job to remove.
 */
void remove_job(pid_t pid){
    for(int i = 0; i < job_count; i++){
        if(jobs[i].pid == pid){
            for(int j = i; j < job_count - 1; j++){
                jobs[j] = jobs[j + 1];
            }
            job_count--;
            break;
        }
    }
}

/**
 * @brief Print the list of currently active jobs.
 */
void print_jobs(){
    for(int i = 0; i < job_count; i++){
        printf("[%d] %s\n", i + 1, jobs[i].cmdline);
    }
}
