/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#ifndef INC_MAIN
#define INC_MAIN

struct main_stack;
struct main_stack_nonstandard;
struct main_stack_semistandard;
struct main_stack_standard;
struct main_work;

typedef int func_main(int, char **);
typedef void func_main_work(struct main_stack *);

struct main_stack_nonstandard
  {
    struct main_stack * self;
    struct main_work ** first_work;
    struct main_work * work;
    int loop;
  };

struct main_stack_semistandard
  {
    int return_value;
  };

struct main_stack_standard
  {
    func_main * p_main;
    int * p_argc;
    char *** p_argv;
  };

struct main_stack
  {
    struct main_stack_standard standard;
    struct main_stack_semistandard semistandard;
    struct main_stack_nonstandard nonstandard;
  };

struct main_work
  {
    func_main_work * work;
  };

#endif /* INC_MAIN */
