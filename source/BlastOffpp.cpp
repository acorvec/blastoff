#include "Program.h"

#ifndef __EMSCRIPTEN__

int main()
{
    BlastOff::Program program;
    while (program.IsRunning())
        program.RunLoopIteration();

    return 0;
}

#else

#include <emscripten.h>

BlastOff::Program program;

static void mainloop()
{
    program.RunLoopIteration();
    if (!program.IsRunning())
        emscripten_cancel_main_loop();
}

int main()
{
    emscripten_set_main_loop(mainloop, 0, 1);
    return 0;
}

#endif