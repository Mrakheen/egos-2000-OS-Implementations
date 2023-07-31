#include "app.h"
#include <stdlib.h>
#include <unistd.h>

int strLen(char* str);
void intToStr(int num, char* str);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        char* usage = "Usage: test_pri <iterations> <priority>\n";
        write(1, usage, 28);
        return 1;
    }

    int iterations = atoi(argv[1]);
    int priority = atoi(argv[2]);

    char pidStr[16];
    int pid = getpid();

    intToStr(pid, pidStr);
    write(1, "PID: ", 5);
    write(1, pidStr, strLen(pidStr));
    write(1, "\n", 1);

    // Set the priority level of the process
    set_prio(pid, priority);

    for (int i = 1; i < iterations; i++) {
        for (int j = 0; j < 100000000; j++) {
            // Delay loop for simulating delay without sleep function
        }
        write(1, "PID: ", 5);
        write(1, pidStr, strLen(pidStr));
        write(1, "\n", 1);
    }

    return 0;
}

int strLen(char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void intToStr(int num, char* str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    if (num < 0) {
        str[i++] = '-';
        num = -num;
    }

    int divisor = 1;
    while (num / divisor > 9) {
        divisor *= 10;
    }

    while (divisor != 0) {
        int digit = num / divisor;
        str[i++] = digit + '0';
        num %= divisor;
        divisor /= 10;
    }

    str[i] = '\0';
}


