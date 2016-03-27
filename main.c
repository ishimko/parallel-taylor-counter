#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <memory.h>
#include <errno.h>

#define ARGS_COUNT 4

char *module_name;

void print_error(const char *module_name, const char *error_msg, const char *file_name) {
    fprintf(stderr, "%s: %s %s\n", module_name, error_msg, file_name ? file_name : "");
}

int main(int argc, char *argv[]) {
    module_name = basename(argv[0]);
    int array_size, taylor_members_count;
    FILE *result_file;

    if (argc != ARGS_COUNT) {
        print_error(module_name, "Wrong number of parameters.", NULL);
        return 1;
    }

    if ((array_size = atoi(argv[1])) < 1){
        print_error(module_name, "Array size must be positive.", NULL);
        return 1;
    };
    if ((taylor_members_count = atoi(argv[2])) < 1){
        print_error(module_name, "Number of taylor members must be positive.", NULL);
        return 1;
    };

    if (!(result_file = fopen(argv[3], "w+"))){
        print_error(module_name, strerror(errno), argv[3]);
        return 1;
    }

    return 0;
}