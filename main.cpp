#include "clock.hpp"
#include "processor.hpp"

extern "C" int main()
{
    clockInit();
    Processor proc;
    while (true) {
        proc.PromptAndExecute();
    }
}