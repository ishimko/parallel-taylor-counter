#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <memory.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>

#define ARGS_COUNT 4

char *module_name;

void print_error(const char *module_name, const char *error_msg, const char *file_name) {
    fprintf(stderr, "%s: %s %s\n", module_name, error_msg, file_name ? file_name : "");
}

double get_sin_taylor_member(double x, int member_number) {
    double result = 1;
    for (int i = 1; i <= member_number * 2 + 1; i++) {
        result *= x / i;
    }
    return (member_number % 2) ? -result : result;
}

void write_result(const int array_size, FILE *tmp_file, FILE *result_file) {
    double *result = alloca(sizeof(double) * array_size);
    memset(result, 0, sizeof(double) * array_size);

    int pid, i;
    double member_value;
    rewind(tmp_file);
    while (!feof(tmp_file)) {
        fscanf(tmp_file, "%d %d %lf", &pid, &i, &member_value);
        result[i] += member_value;
    }

    for (i = 0; i < array_size; i++) {
        fprintf(result_file, "y[%d]=%lf\n", i, result[i]);
    }

}

void count_function_values(const int array_size, const int taylor_members_count, const char *result_path) {
    FILE *result_file, *tmp_file;
    if (!(result_file = fopen(result_path, "w+"))) {
        print_error(module_name, strerror(errno), result_path);
        return;
    }

    if (!(tmp_file = tmpfile())) {
        print_error(module_name, strerror(errno), NULL);
        return;
    }

    pid_t pid;
    for (int i = 0; i < array_size; i++) {
        double x = M_PI - (2 * M_PI * i) / array_size;
        for (int j = 0; j < taylor_members_count; j++) {
            pid = fork();
            if (pid == 0) {
                double member = get_sin_taylor_member(x, j);
                fprintf(tmp_file, "%d %d %lf\n", getpid(), i, member);
                printf("%d %d %lf\n", getpid(), i, member);
                return;
            } else if (pid == -1) {
                print_error(module_name, strerror(errno), NULL);
                return;
            }
        }
        while (wait(NULL) > 0) {
        }
    }
    write_result(array_size, tmp_file, result_file);
}

int main(int argc, char *argv[]) {
    module_name = basename(argv[0]);
    int array_size, taylor_members_count;

    if (argc != ARGS_COUNT) {
        print_error(module_name, "Wrong number of parameters.", NULL);
        return 1;
    }

    if ((array_size = atoi(argv[1])) < 1) {
        print_error(module_name, "Array size must be positive.", NULL);
        return 1;
    };
    if ((taylor_members_count = atoi(argv[2])) < 1) {
        print_error(module_name, "Number of taylor members must be positive.", NULL);
        return 1;
    };

    count_function_values(array_size, taylor_members_count, argv[3]);

    return 0;
}