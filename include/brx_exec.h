#ifndef BRX_EXEC_H
#define BRX_EXEC_H

#include <stdint.h>

typedef enum {
    BRX_EXEC_UNKNOWN = 0,
    BRX_EXEC_BRX_ELF,
    BRX_EXEC_LINUX_ELF,
    BRX_EXEC_WINDOWS_PE,
    BRX_EXEC_SCRIPT,
    BRX_EXEC_PACKAGE
} brx_exec_format_t;

typedef enum {
    BRX_EXEC_BACKEND_NONE = 0,
    BRX_EXEC_BACKEND_NATIVE,
    BRX_EXEC_BACKEND_LINUX_COMPAT,
    BRX_EXEC_BACKEND_WINDOWS_SOURCE,
    BRX_EXEC_BACKEND_INTERPRETER,
    BRX_EXEC_BACKEND_PACKAGE
} brx_exec_backend_t;

typedef struct {
    brx_exec_format_t format;
    brx_exec_backend_t backend;
    uint64_t supported;
    const char *description;
} brx_exec_plan_t;

brx_exec_format_t brx_exec_detect(const void *image, uint64_t length);
int brx_exec_validate(const void *image, uint64_t length, brx_exec_format_t format);
brx_exec_plan_t brx_exec_plan(brx_exec_format_t format);
const char *brx_exec_format_name(brx_exec_format_t format);
const char *brx_exec_backend_name(brx_exec_backend_t backend);

#endif
