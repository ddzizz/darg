// darg.h
#ifndef DARG_H
#define DARG_H

#ifdef DARG_IMPLEMENTATION

#ifndef DARG_API
#ifdef DARG_PRIVATE
#if (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199409L))
#define DARG_API static inline
#elif defined(__cplusplus)
#define DARG_API static inline
#else
#define DARG_API static
#endif
#else
#define DARG_API extern
#endif
#endif

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

// 配置宏（用户可自定义）
#ifndef DARG_MAX_CMDS
#define DARG_MAX_CMDS 32
#endif

#ifndef DARG_MAX_OPTS
#define DARG_MAX_OPTS 16
#endif

#ifndef DARG_MAX_ARGS
#define DARG_MAX_ARGS 16
#endif

#ifndef DARG_MAX_FLAGS
#define DARG_MAX_FLAGS 16
#endif

#ifndef DARG_MAX_CMD_OPTS
#define DARG_MAX_CMD_OPTS 16
#endif

#ifndef DARG_MAX_CMD_FLAGS
#define DARG_MAX_CMD_FLAGS 16
#endif

    typedef struct darg_option
    {
        int type_id;
        char short_name;
        const char *long_name;
        const char *description;
        const char *value;
        bool present;
    } darg_option_t;

    typedef struct darg_flag
    {
        int type_id;
        char short_name;
        const char *long_name;
        const char *description;
        bool present;
    } darg_flag_t;

    typedef struct darg_command
    {
        int type_id;
        const char *name;
        const char *description;
        bool present;

        // 命令专属选项
        darg_option_t opts[DARG_MAX_CMD_OPTS];
        size_t opt_count;

        // 命令专属开关
        darg_flag_t flags[DARG_MAX_CMD_FLAGS];
        size_t flag_count;
    } darg_command_t;

    typedef struct darg
    {
        // 应用信息
        const char *app_name;
        const char *app_desc;
        const char *version;

        // 全局选项
        darg_option_t global_opts[DARG_MAX_OPTS];
        size_t global_opt_count;

        // 全局开关
        darg_flag_t global_flags[DARG_MAX_FLAGS];
        size_t global_flag_count;

        // 命令集合
        darg_command_t commands[DARG_MAX_CMDS];
        size_t cmd_count;

        int active_cmd_id;  // 类型ID而非数组索引
        bool has_any_match; // 是否解析到任何有效参数
    } darg_t;

    DARG_API bool darg_add_opt(darg_t *p, int id, char sname, const char *lname, const char *desc);
    DARG_API bool darg_add_flag(darg_t *p, int id, char sname, const char *lname, const char *desc);
    DARG_API bool darg_add_cmd(darg_t *p, int id, const char *name, const char *desc);
    DARG_API bool darg_add_cmd_opt(darg_t *p, int cmd_id, int opt_id, char sname, const char *lname, const char *desc);
    DARG_API bool darg_add_cmd_flag(darg_t *p, int cmd_id, int flag_id, char sname, const char *lname, const char *desc);
    DARG_API bool darg_parse(darg_t *p, int argc, const char **argv);
    DARG_API const char *darg_get_opt(darg_t *p, int id);
    DARG_API bool darg_has_flag(darg_t *p, int id);
    DARG_API int darg_get_cmd(const darg_t *p);
    DARG_API const char *darg_get_cmd_opt(darg_t *p, int cmd_id, int opt_id);
    DARG_API bool darg_has_cmd_flag(darg_t *p, int cmd_id, int flag_id);
    DARG_API void darg_print_version(const darg_t *p);
    DARG_API void darg_print_help(const darg_t *p);

#ifdef __cplusplus
}
#endif
/*---------------- 宏定义 ----------------*/

#define DARG_DECLARE(name, appname, desc, ver) \
    darg_t name = {                            \
        .app_name = appname,                   \
        .app_desc = desc,                      \
        .version = ver,                        \
        .active_cmd_id = -1,                   \
        .has_any_match = false}

/*---------------- 基础接口 ----------------*/
// 获取激活的命令ID
bool darg_add_cmd(darg_t *p, int id, const char *name, const char *desc)
{
    if (p->cmd_count >= DARG_MAX_CMDS)
        return false;
    p->commands[p->cmd_count++] = (darg_command_t){
        .type_id = id,
        .name = name,
        .description = desc,
        .opt_count = 0,
        .flag_count = 0};
    return true;
}

bool darg_add_opt(darg_t *p, int id, char sname,
                  const char *lname, const char *desc)
{
    if (p->global_opt_count >= DARG_MAX_OPTS)
        return false;
    p->global_opts[p->global_opt_count++] = (darg_option_t){
        id, sname, lname, desc, NULL, false};
    return true;
}

bool darg_add_flag(darg_t *p, int id, char sname,
                   const char *lname, const char *desc)
{
    if (p->global_flag_count >= DARG_MAX_FLAGS)
        return false;
    p->global_flags[p->global_flag_count++] = (darg_flag_t){
        id, sname, lname, desc, false};
    return true;
}

/*---------------- 命令专属接口 ----------------*/
bool darg_add_cmd_opt(darg_t *p, int cmd_id, int opt_id,
                      char sname, const char *lname, const char *desc)
{
    for (size_t i = 0; i < p->cmd_count; ++i)
    {
        if (p->commands[i].type_id == cmd_id)
        {
            if (p->commands[i].opt_count >= DARG_MAX_CMD_OPTS)
                return false;
            p->commands[i].opts[p->commands[i].opt_count++] = (darg_option_t){
                opt_id, sname, lname, desc, NULL, false};
            return true;
        }
    }
    return false;
}

bool darg_add_cmd_flag(darg_t *p, int cmd_id, int flag_id,
                       char sname, const char *lname, const char *desc)
{
    for (size_t i = 0; i < p->cmd_count; ++i)
    {
        if (p->commands[i].type_id == cmd_id)
        {
            if (p->commands[i].flag_count >= DARG_MAX_CMD_FLAGS)
                return false;
            p->commands[i].flags[p->commands[i].flag_count++] = (darg_flag_t){
                flag_id, sname, lname, desc, false};
            return true;
        }
    }
    return false;
}

/*---------------- 解析逻辑 ----------------*/
darg_option_t *darg_match_opt(darg_option_t *opts, int count, const char *arg)
{
    bool is_long = (arg[1] == '-');
    const char *target = is_long ? arg + 2 : arg + 1;

    for (size_t i = 0; i < count; ++i) 
    {
        darg_option_t *opt = &opts[i];
        int len = is_long? strlen(opt->long_name) : 1;
        if ((is_long && opt->long_name && strncmp(target, opt->long_name, len) == 0) ||
            (!is_long && *target == opt->short_name))
        {
            if (target[len] == '=')
            {
                opt->value = target + len + 1;
            }
            opt->present = true;
            return opt;
        }
    }
    return NULL;
}

darg_flag_t *darg_match_flag(darg_flag_t *flags, int count, const char *arg)
{
    bool is_long = (arg[1] == '-');
    const char *target = is_long ? arg + 2 : arg + 1;

    for (size_t j = 0; j < count; ++j)
    {
        darg_flag_t *flag = &flags[j];
        if ((is_long && flag->long_name && strcmp(target, flag->long_name) == 0) ||
            (!is_long && *target == flag->short_name))
        {
            flag->present = true;
            return flag;
        }
    }
    return NULL;
}

darg_command_t *darg_match_cmd(darg_command_t *cmds, int count, const char *arg)
{
    for (size_t i = 0; i < count; ++i)
    {
        darg_command_t *cmd = &cmds[i];
        if (strcmp(arg, cmd->name) == 0)
        {
            cmd->present = true;
            // p->active_cmd_id = p->commands[i].type_id; // 存储类型ID
            // p->has_any_match = true;
            return cmd;
        }
    }
    return NULL;
}

bool darg_match_opt_or_flag(darg_option_t *opts, int opt_count, darg_flag_t *flags, int flag_count, int i, int argc, const char **argv)
{
    darg_option_t *opt = NULL;
    darg_flag_t *flag = NULL;
    for (; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (arg[0] != '-')
        {
            continue;
        }

        flag = darg_match_flag(flags, flag_count, arg);
        if (flag != NULL)
        {
            continue;
        }
        
        opt = darg_match_opt(opts, opt_count, arg);
        if (opt != NULL)
        {
            if (!opt->value && i + 1 < argc && argv[i + 1][0]!= '-')
            {
                opt->value = argv[i + 1];
            }
            // i++;
            // while (i < argc && argv[i][0]!= '-')
            // {
            //     const char *arg = argv[i];
            //     opt->value = arg; 
            //     ++i;
            // }
            continue;
        }
        printf("unknown option '%s'\n", arg);
        return false;
    }

    return true;
}

// 规则：
// [全局option | flag] <command> [<args>]
bool darg_parse(darg_t *p, int argc, const char **argv)
{
    bool is_command = argv[1][0] != '-';
    if (is_command)
    {
        darg_command_t *cmd = darg_match_cmd(p->commands, p->cmd_count, argv[1]); 
        if (cmd == NULL)
        {
            printf("unknown command '%s'\n", argv[1]); 
            return false;
        }

        p->active_cmd_id = cmd->type_id;
        
        if (!darg_match_opt_or_flag(cmd->opts, cmd->opt_count, cmd->flags, cmd->flag_count, 2, argc, argv))
        {
            return false;
        }
    }
    else
    {
        if (!darg_match_opt_or_flag(p->global_opts, p->global_opt_count, p->global_flags, p->global_flag_count, 1, argc, argv))
        {
            return false;
        }
    }

    return true;
}

const char *darg_get_opt(darg_t *p, int id)
{
    for (size_t i = 0; i < p->global_opt_count; ++i)
    {
        if (p->global_opts[i].type_id == id)
        {
            return p->global_opts[i].value;
        }
    }
    return NULL;
}

bool darg_has_flag(darg_t *p, int id)
{
    for (size_t i = 0; i < p->global_flag_count; ++i)
    {
        if (p->global_flags[i].type_id == id)
        {
            return p->global_flags[i].present;
        }
    }
    return false;
}

int darg_get_cmd(const darg_t *p)
{
    return p->active_cmd_id;
}

const char *darg_get_cmd_opt(darg_t *p, int cmd_id, int opt_id)
{
    for (size_t i = 0; i < p->cmd_count; ++i)
    {
        if (p->commands[i].type_id == cmd_id)
        {
            for (size_t j = 0; j < p->commands[i].opt_count; ++j)
            {
                if (p->commands[i].opts[j].type_id == opt_id)
                {
                    return p->commands[i].opts[j].value;
                }
            }
        }
    }
    return NULL;
}

bool darg_has_cmd_flag(darg_t *p, int cmd_id, int flag_id)
{
    for (size_t i = 0; i < p->cmd_count; ++i)
    {
        if (p->commands[i].type_id == cmd_id)
        {
            for (size_t j = 0; j < p->commands[i].flag_count; ++j)
            {
                if (p->commands[i].flags[j].type_id == flag_id)
                {
                    return p->commands[i].flags[j].present;
                }
            }
        }
    }
    return false;
}

/*---------------- 帮助输出 ----------------*/
void darg_print_version(const darg_t *p)
{
    printf("%s %s\n", p->app_name, p->version);
}

void darg_print_help(const darg_t *p)
{
    printf("%s %s\n\n", p->app_name, p->version);
    printf("usage: %s [COMMAND] [OPTIONS]\n\n", p->app_name);
    printf("description:\n  %s\n\n", p->app_desc);

    // 全局选项
    if (p->global_opt_count > 0 || p->global_flag_count > 0)
    {
        printf("options:\n");
        for (size_t i = 0; i < p->global_opt_count; ++i)
        {
            const darg_option_t *opt = &p->global_opts[i];
            printf("  -%c, --%-12s%s\n",
                   opt->short_name,
                   opt->long_name,
                   opt->description);
        }
        for (size_t i = 0; i < p->global_flag_count; ++i)
        {
            const darg_flag_t *flag = &p->global_flags[i];
            printf("  -%c, --%-12s%s\n",
                   flag->short_name,
                   flag->long_name,
                   flag->description);
        }
        printf("\n");
    }

    // 命令列表
    if (p->cmd_count > 0)
    {
        printf("commands:\n");
        for (size_t i = 0; i < p->cmd_count; ++i)
        {
            const darg_command_t *cmd = &p->commands[i];
            printf("  %-12s%s\n", cmd->name, cmd->description);

            // 命令专属选项
            if (cmd->opt_count > 0 || cmd->flag_count > 0)
            {
                printf("    options:\n");
                for (size_t j = 0; j < cmd->opt_count; ++j)
                {
                    const darg_option_t *opt = &cmd->opts[j];
                    printf("      -%c, --%-10s%s\n",
                           opt->short_name,
                           opt->long_name,
                           opt->description);
                }
                for (size_t j = 0; j < cmd->flag_count; ++j)
                {
                    const darg_flag_t *flag = &cmd->flags[j];
                    printf("      -%c, --%-10s%s\n",
                           flag->short_name,
                           flag->long_name,
                           flag->description);
                }
            }
        }
    }
}
#endif // DARG_IMPLEMENTATION

#endif // DARG_H
