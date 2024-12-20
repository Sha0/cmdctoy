#!/bin/sh

mkdir bin/ 2> /dev/null

# Build the core program:
gcc -ansi -pedantic -Wall -Wextra -Werror -g -o bin/cmdctoy -D CMDCTOY_POSIX=1 btree.c builtins.c cmd_exit.c cmd_help.c cmd_hexd.c cmd_load.c cmd_mono.c cmd_type.c command.c depend.c gui.c list.c main.c main1st.c mod2.c module.c process.c stage2.c toy.c toyio.c toylib.c toyscope.c type.c -ldl

# As example items from the builtins, rebuild these loadable modules, too:
gcc -ansi -pedantic -Wall -Wextra -Werror -shared -g -o bin/gui.so -fPIC -D BUILTIN_GET_USER_INPUT=0 gui.c
gcc -ansi -pedantic -Wall -Wextra -Werror -shared -g -o bin/cmd_help.so -fPIC -D BUILTIN_CMD_HELP=0 cmd_help.c
