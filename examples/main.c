#define DARG_IMPLEMENTATION 1

#include "../darg.h"

enum {
    FLAG_HELP = 1,
    FLAG_VERSION,

    CMD_BUILD,
    CMD_RUN,

    CMD_BUILD_OPT_OUTPUT,
    CMD_BUILD_FLAG_DEBUG,

    CMD_RUN_OPT_OUTPUT,
};

void on_cmd_build(darg_t* parser) {
    const char* output = darg_get_cmd_opt(parser, CMD_BUILD, CMD_BUILD_OPT_OUTPUT);
    bool debug = darg_has_cmd_flag(parser, CMD_BUILD, CMD_BUILD_FLAG_DEBUG);
    
    printf("Building project...\n");
    if (debug) printf("Debug mode enabled\n");
    printf("Output path: %s\n", output ? output : "default");
}

void on_cmd_run(darg_t* parser) {
    const char* output = darg_get_cmd_opt(parser, CMD_RUN, CMD_RUN_OPT_OUTPUT);
    printf("Running project...\n");
    printf("Output path: %s\n", output? output : "default");
}

void on_default(darg_t* parser) {
    if (darg_has_flag(parser, FLAG_HELP)) {
        darg_print_help(parser);
        return;
    }

    if (darg_has_flag(parser, FLAG_VERSION)) {
        darg_print_version(parser);
        return;
    }

    darg_print_help(parser);
}

int main(int argc, char** argv) {
    DARG_DECLARE(parser, "Builder", "Project builder", "v2.1");
    
    // 添加全局选项
    darg_add_flag(&parser, FLAG_HELP, 'h', "help", "Show help");
    darg_add_flag(&parser, FLAG_VERSION, 'v', "version", "Show version");
    
    // 添加构建命令
    darg_add_cmd(&parser, CMD_BUILD, "build", "Build project");
    darg_add_cmd_opt(&parser, CMD_BUILD, CMD_BUILD_OPT_OUTPUT, 'o', "output", "Output path");
    darg_add_cmd_flag(&parser, CMD_BUILD, CMD_BUILD_FLAG_DEBUG, 'd', "debug", "Debug mode");

    // 添加运行命令
    darg_add_cmd(&parser, CMD_RUN, "run", "Run project");
    darg_add_cmd_opt(&parser, CMD_RUN, CMD_RUN_OPT_OUTPUT, 'o', "output", "Output path");
    
    darg_parse(&parser, argc, (const char**)argv);
    
    switch (darg_get_cmd(&parser)) {
        case CMD_BUILD: {
            on_cmd_build(&parser);
            break;
        }
        case CMD_RUN: {
            on_cmd_run(&parser);
            break;
        }
        default: {
            on_default(&parser);
            break;
        } 
    }
    
    return 0;
}
