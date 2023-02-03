#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sched.h>
#include <signal.h>
#include <time.h>

volatile int interrupt_occurred = 0;

void interrupt_handler(int signum) {
  interrupt_occurred = 1;
}

int main(void) {
    FILE *fp;
    char *soc_name = NULL;
    char* revision = NULL;
    char* model = NULL;
    size_t n = 0;
    char *line;
    int rev_int = 0;
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(0, &set);
    struct timeval start, end;
    int first_pipefd[2], second_pipefd[2];
    size_t nloops = 1000000;

    fp = fopen("/proc/cpuinfo", "r");
    assert(fp != NULL);

    while (getline(&line,&n, fp) > 0) {
        if (strncmp(line, "Hardware", 8) == 0) {
            soc_name = strchr(line, ':') + 2;
            printf("SoC name: %s\n", soc_name);
            continue;
        }

        if (strncmp(line, "Revision", 8) == 0) {
            revision = strchr(line, ':') + 2;
            printf("revision: %s\n", revision);
            continue;
        }

        if (strncmp(line, "Model", 5) == 0) {
            model = strchr(line, ':') + 17;
            printf("version: %s\n", model);
            break;
        }

    }

    /* Convert revision code to integer */
    sscanf(revision, "%x", &rev_int);

    /* Check the manufacturer */
    //TODO: Investigate further
    if ((rev_int & 0xF000) == 0x5000) {
        printf("Manufacturer: Sony UK\n");
    } else if ((rev_int & 0xF000) == 0x3000) {
        printf("Manufacturer: Egoman\n");
    } else {
        printf("Manufacturer: Unknown\n");
    }

    /*Generate the pin layout*/
    system("pinout");

    printf("\n\nOS details: \n");
    /*Name and release version of the operating system*/
    system("cat /etc/os-release");


    
    printf("\n\nBegin calculating context switch time \n");
    if (pipe(first_pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(second_pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid_t cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (cpid == 0) {    // child
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1) {
            exit(EXIT_FAILURE);
        }
        for (size_t i = 0; i < nloops; i++) {
            read(first_pipefd[0], NULL, 0);
            write(second_pipefd[1], NULL, 0);
        }
    } else {        // parent
        if (sched_setaffinity(getpid(), sizeof(cpu_set_t), &set) == -1) {
            exit(EXIT_FAILURE);
        }
        gettimeofday(&start, NULL);
        for (size_t i = 0; i < nloops; i++) {
            write(first_pipefd[1], NULL, 0);
            read(second_pipefd[0], NULL, 0);
        }
        gettimeofday(&end, NULL);
        printf("context switch: %f microseconds\n", (float) (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / nloops);
    }

    printf("\n\nSend a SIGINT signal by pressing CTRL+C \n");
    signal(SIGINT, interrupt_handler);
    gettimeofday(&start, NULL);
    while (!interrupt_occurred) {};
    gettimeofday(&end, NULL);
    printf("interrupt latency: %f microseconds\n", (float) (end.tv_sec * 1000000 + end.tv_usec - start.tv_sec * 1000000 - start.tv_usec) / nloops);



    fclose(fp);
    return 0;
}