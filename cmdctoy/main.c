/*
 * Copyright (C) 2024 Shao Miller.  All rights reserved.
 */
#include <stdlib.h>
#include "main.h"
#include "main1st.h"

func_main main;

int main(int argc, char ** argv)
  {
    struct main_stack stack;

    stack.standard.p_main = &main;
    stack.standard.p_argc = &argc;
    stack.standard.p_argv = &argv;
    stack.semistandard.return_value = EXIT_SUCCESS;
    stack.nonstandard.self = &stack;
    stack.nonstandard.first_work = &main_first_work;
    stack.nonstandard.work = main_first_work;
    stack.nonstandard.loop = 0;

    /* Permit stack to be "swapped out" as a whole instead of in parts, via self */
    do stack.nonstandard.self->nonstandard.work->work(stack.nonstandard.self); while (stack.nonstandard.self->nonstandard.loop);

    return stack.nonstandard.self->semistandard.return_value;
  }
