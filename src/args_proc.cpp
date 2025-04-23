#include <cstddef>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "args_proc.h"

void scan_argv(config_t *config, const int argc, const char *argv[]) {
    assert(config);

    // DEFAULT OPTIONS
    snprintf(config->hash_func_name, MAX_CONFIG_NAME_SIZE - 1, DEFAULT_CONFIG_HASH_FUNC);
    snprintf(config->benchmark, MAX_CONFIG_NAME_SIZE - 1, DEFAULT_CONFIG_BENCHMARK);
    config->measures_cnt = DEFAULT_CONFIG_MEASURE_CNT;
    config->print_state = 1;

    opt_data options[] =
    {
        {"-r", "--runs", "%d", &config->measures_cnt},
        {"-h", "--hash_func", "%s", config->hash_func_name},
        {"-o", "--output", "%s", &config->output_path},
        {"-b", "--benchmark", "%s", &config->benchmark},
        {"-p", "--print", "%d", &config->print_state}
    };

    size_t n_options = sizeof(options) / sizeof(opt_data);

    get_options(argc, argv, options, n_options);
}

opt_data *option_list_ptr(const char *name, opt_data opts[], const size_t n_opts) {
    for (size_t i = 0; i < n_opts; i++) {
        if (strcmp(name, opts[i].short_name) == 0 || strcmp(name, opts[i].long_name) == 0) {
            return &opts[i];
        }
    }
    return NULL;
}

void get_options(const int argc, const char* argv[], opt_data opts[], const size_t n_opts) {
    assert(argc >= 0);

    for (int i = 1; i < argc; i++) {
        char name[MAX_OPT_NAME_SZ];
        char value[MAX_OPT_NAME_SZ];
        sscanf(argv[i], "%[^=]%s", name, value);

        opt_data *ptr = option_list_ptr(name, opts, n_opts);

        if (ptr != NULL) {
            sscanf(value + 1, (ptr->fmt), ptr->val_ptr);
            ptr->exist = true;
        }
    }
}
