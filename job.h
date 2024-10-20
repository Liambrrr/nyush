#ifndef JOB_H
#define JOB_H

#include <sys/types.h>

#define MAX_JOBS 100
#define MAX_CMD_LENGTH 1000

typedef struct Job {
    pid_t pid;
    char cmdline[MAX_CMD_LENGTH];
} Job;

extern Job jobs[MAX_JOBS];
extern int job_count;

void add_job(pid_t pid, char* cmdline);
void remove_job(pid_t pid);
void print_jobs();

#endif
