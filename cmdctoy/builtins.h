/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_BUILTINS
#define INC_BUILTINS

#include "toy.h"

extern int builtin_startup(struct top *);

#ifndef BUILTIN_CMD_EXIT
#define BUILTIN_CMD_EXIT 1
#endif
#ifndef BUILTIN_CMD_HELP
#define BUILTIN_CMD_HELP 1
#endif
#ifndef BUILTIN_CMD_HEXDUMP
#define BUILTIN_CMD_HEXDUMP 1
#endif
#ifndef BUILTIN_CMD_LOAD
/* TODO: Support non-POSIX */
#if CMDCTOY_POSIX
#define BUILTIN_CMD_LOAD 1
#else
#define BUILTIN_CMD_LOAD 0
#endif /* CMDCTOY_POSIX */
#endif /* BUILTIN_CMD_LOAD */
#ifndef BUILTIN_CMD_MONO
#define BUILTIN_CMD_MONO 1
#endif
#ifndef BUILTIN_CMD_TYPE
#define BUILTIN_CMD_TYPE 1
#endif
#ifndef BUILTIN_GET_USER_INPUT
#define BUILTIN_GET_USER_INPUT 1
#endif
#ifndef BUILTIN_MOD2
#define BUILTIN_MOD2 1
#endif

#endif /* INC_BUILTINS */
