#include "Program.h"

int main()
{
    BlastOff::Program program;
    while (program.IsRunning())
        program.RunLoopIteration();

    return 0;
}