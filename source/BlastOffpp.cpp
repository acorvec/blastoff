#include "Program.h"

#if COMPILE_TARGET_DESKTOP

BlastOff::Program program;

int main()
{
    BlastOff::SetUpPlatform(program.GetTargetFramerate());

    while (program.IsRunning())
        program.RunLoopIteration();

    return 0;
}

#elif COMPILE_TARGET_EMSCRIPTEN

#include <emscripten.h>

BlastOff::Program program;

void mainloop()
{
    program.RunLoopIteration();
    if (!program.IsRunning())
        emscripten_cancel_main_loop();
}

int main()
{
    const auto windowSizeCallback = 
        [](const int width, const int height)
        {
            program.SetWindowSize(width, height);
        };

    BlastOff::Emscripten::SetWindowSizeCallback(windowSizeCallback);
    BlastOff::Emscripten::SetAspectRatio(9 / 16.0f);

    BlastOff::SetUpPlatform(program.GetTargetFramerate());

    return 0;
}

#endif
