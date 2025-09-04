#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/wait.h>

int main() {
    int p1[2]; // parent -> child
    int p2[2]; // child -> parent
    pipe(p1);
    pipe(p2);

    pid_t pid = fork();

    int n = 1000000; // 1 million integers
    struct timeval start, end;

    if (pid == 0) {
        // Child process
        int num;
        for (int i = 0; i < n; i++) {
            read(p1[0], &num, sizeof(int)); // read from parent
            write(p2[1], &num, sizeof(int)); // send back to parent
        }
    } else {
        // Parent process
        int num;
        gettimeofday(&start, NULL); // start time

        for (int i = 0; i < n; i++) {
            num = i;
            write(p1[1], &num, sizeof(int)); // send to child
            read(p2[0], &num, sizeof(int));  // read back from child
        }

        gettimeofday(&end, NULL); // end time

        // Calculate total time in seconds
        double total_time = (end.tv_sec - start.tv_sec) + 
                            (end.tv_usec - start.tv_usec) / 1000000.0;

        printf("Total time: %f seconds\n", total_time);
        printf("Requests per second: %f\n", n / total_time);
        printf("Average roundtrip time: %f microseconds\n", (total_time / n) * 1000000);
        printf("Time for 1 forwarding: %f microseconds\n", ((total_time / n) * 1000000) / 2);

        wait(NULL); // wait for child
    }

    return 0;
}