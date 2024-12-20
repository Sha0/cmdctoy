Copyright (C) 2024 Shao Miller.  All rights reserved.

Description:

This project is a work in progress that was started on December 1st, 2024.  It isn't yet ready
for general usage, so for now, you'll have to figure it out on your own.

Building:

This 'readme.txt' file is in a 'cmdctoy/' directory.
To build with an 'sh' script and with 'gcc', go into the 'cmdctoy/' directory, then run the
'./build.sh' script.  After that, go into the newly-created 'bin/' directory and find the
'cmdctoy' program.
To build with a disk operating system and with an "open" compiler, use a batch-file provided
by the compiler to set up environment-variables, then go into the 'cmdctoy/' directory and run
'wcl -fe=cmdctoy -i=. *.c', then find the 'cmdctoy.exe' program.
This program accepts commands from standard input.
This program has a 'help' command and an 'exit' command.

Portability:

A compiler having "Wat" in its name appears to make macros for 'feof' and 'ferror', so that is why
we have 'api_stdio->f_eof' instead of 'api_stdio->feof' and 'api_stdio->f_error' instead of
'api_stdio->ferror'.  The alternative would be to have '#undef' applied to those macros, in gui.c.

To-dos:

Multiple, including:
- Take the thread precursor from toy.c and make something decent out of it
  - Ideally, threads can be uniprocessor and co-operative until a non-portable module facility for
    a given OS/environment can "take over" the threads, at some point after start-up, if such a
    facility is available and appropriate
- Keep everything as C89 as possible and continue to modularize what isn't C89
- Add a way to populate arbitrary blobs of data into allocated storage
- Add some basic math
- Improve the module protocol
- Expose more C functions with commands
- Saving and restoring stuff
- Add other dynamic linking support

Legal:

"POSIX" is a trade-mark of the Institute of Electrical and Electronic Engineers (IEEE).  It is
referenced by this project as a "hint" in various files, but this project is not certified by the
IEEE and this project currently makes no claim of anything bearing any good will nor bad will that
might be associated with that trade-mark.  Please do not make any assumptions that the mentions
of "POSIX" in this source-code mean anything to you, nor do they carry any expression of value.

Also see the top line of this file.  The legal situation might change, in the future.
