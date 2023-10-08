#include <conio.h> // getch
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vector>

#include "errors.h"
#include "mymemory.h"
#include "mytypes.h"
#include "scanner.h"
#include "tokens.h"


static bool runFile(const char* filename)
{
    printf("Filename: %s\n", filename);

    if(filename == nullptr)
    {
        LOG_ERROR("Filename is nullptr");
        return false;
    }

    FILE* file = fopen(filename, "rb");
    if(file == nullptr)
    {
        LOG_ERROR("Failed to open file.");
        return false;
    }
    MyMemory mem;

    fseek(file, 0L, SEEK_END);
    size_t sz = ftell(file);
    fseek(file, 0L, SEEK_SET);

    mem.scriptFileData.resize(sz + 1);
    fread(mem.scriptFileData.data(), 1, sz, file);
    mem.scriptFileData[sz] = '\0';
    fclose(file);

    if(!scanner_run(mem))
    {
        printf("Some failure in: %s\n", filename);
    }
    printf("%s\n", mem.scriptFileData.data());

    return true;
}



static void runPrompt()
{
}

int main(int argc, const char** argv)
{
    if(argc > 3)
    {
        printf("Usage: carp [script]\n");
        return 64;
    }
    else if(argc == 2)
    {
        if(!runFile(argv[1]))
        {
            printf("Failed to run file: %s\n", argv[1]);
        }
    }
    else
    {
        const char* filename = "progs/print.carp";
        if(!runFile(filename))
        {
            printf("Failed to run file: %s\n", filename);
        }
        printf("Script ran!\n");
        //runPrompt();
    }
    printf("Wait until key pressed!\n");
    getch();
    return 0;
}
