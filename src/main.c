#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILEPATH_MAX_SIZE 128llu
#define MAX_LINE_SIZE

typedef unsigned char bool;
#define true 1
#define false 0

typedef unsigned char byte;

enum Mode
{
    MODE_BINARY,
    MODE_ASCII,

    MODE_COUNT
};

void printchar(char c, bool spaces)
{
    // ignore non printable characters
    if (c < 31)
    {
        if (spaces) printf(" . ");
        else printf(".");
    }
    else 
    {
        if (spaces) printf(" %c ", c);
        else printf("%c", c);
    }
}

bool hexdump(FILE* file, size_t line_size, unsigned char mode, bool col_indicies, bool extended)
{
    size_t filesize = 0llu; // file size in bytes

    printf("line size: %zu\n", line_size);

    if (line_size <= 0)
    {
        fprintf(stderr, "Invalid line size %zu\n", line_size);
        exit(1);
    }

    byte* line = NULL;
    line = calloc(line_size, sizeof(byte));

    if (line == NULL)
    {
        fprintf(stderr, "Failed to allocate line buffer\n");
        exit(1);
    }

    fseek(file, 0, SEEK_END); // find end of file
    filesize = ftell(file);
    fseek(file, 0, SEEK_SET); // back to beggining of file

    size_t num_lines = (size_t)(filesize / line_size) + 1;

    printf("[mode %u]: ", mode);

    if (col_indicies)
    {
        for (size_t i = 0; i < line_size; i++)
        {
            printf("%02zX ", i);
        }
    }

    printf("\n");
    
    switch (mode) 
    {
        case MODE_BINARY:
        {
            for (size_t i = 0; i < num_lines; i += 1)
            {
                fread(line, sizeof(byte), line_size, file); 

                printf("%08zX: ", i * line_size);

                for (size_t j = 0; j < line_size; j++)
                {
                    printf("%02hhX ", line[j]);
                }

                if (extended)
                {
                    printf("   | ");
                    for (size_t j = 0; j < line_size; j++)
                    {
                        printchar(line[j], 0);
                    }
                    printf(" | ");
                }

                printf("\n");
            }
            break;
        }

        case MODE_ASCII:
        {
            for (size_t i = 0; i < num_lines; i += 1)
            {
                fread(line, sizeof(byte), line_size, file); 

                printf("%08zX: ", i * line_size);

                for (size_t j = 0; j < line_size; j++)
                {
                    printchar(line[j], col_indicies);
                }

                if (extended)
                {
                    printf("   | ");
                    for (size_t j = 0; j < line_size; j++)
                    {
                        printf("%02hhX ", line[j]);
                    }
                    printf(" | ");
                }

                printf("\n");
            }
            break;
        }

        default:
        {
            fprintf(stderr, "Invalid mode %u\n", mode);
            free(line);
            return false;
        }
    }

    free(line);
    return true;   
}

void usage(void)
{
    printf("Usage:\n");
    printf("    hxdump <file/filepath> <arguments> <flags>\n");
    printf("\n");
    printf("    arguments:\n");
    printf("     -h         prints this message\n");
    printf("     -l <num>   changes size of line\n");
    printf("\n");
    printf("    flags:\n");
    printf("      modes:\n");
    printf("       --binary   open files in binary mode (mode 0)\n");
    printf("       --ascii    open files in ascii mode  (mode 1)\n");
    printf("\n");
    printf("      other:\n");
    printf("       --nocol     no column indicies\n");
    printf("       --extended  print ascii/binary representation\n");
    printf("\n");
}

int main(int argc, char**argv)
{
    if (argc <= 1)
    {
        fprintf(stderr, "No file provided\n\n");
        usage();
        exit(1);
    }

    if (strncmp(argv[1], "-h", 32) == 0)
    {
        usage();
        exit(0);
    }

    char filepath[FILEPATH_MAX_SIZE];

    memcpy(filepath, argv[1], strnlen(argv[1], FILEPATH_MAX_SIZE));

    FILE* file = fopen(filepath, "r");

    if (file == NULL)
    {
        if ((strnlen(argv[1], 256) * sizeof(char)) >= FILEPATH_MAX_SIZE)
        {
            fprintf(stderr, "Filepath too large (over %llu characters)\n", FILEPATH_MAX_SIZE);
            fprintf(stderr, "Filepath provided: %s\n", argv[1]);
            exit(1);
        }
        fprintf(stderr, "File (%s) either does not exist or is inaccessible\n", filepath);
        exit(1);
    }

    // reaching this point means the file *should* be valid, congratulations!


    // flag parsing
    unsigned char mode = MODE_BINARY;
    bool col = true;
    bool extended = false;    

    const size_t max_read = 12;
    if (argc > 3)
    {
        for (int i = 0; i < argc; i++)
        {
            if (strncmp(argv[i], "--binary", max_read) == 0)
            {
                mode = MODE_BINARY;
            }
            else if (strncmp(argv[i], "--ascii", max_read) == 0)
            {
                mode = MODE_ASCII;
            }
            else if (strncmp(argv[i], "--nocol", max_read) == 0)
            {
                col = false;
            }
            else if (strncmp(argv[i], "--extended", max_read) == 0)
            {
                extended = true;
            }
        }
    }

    int line_size = 16;

    // argument parsing

    if (argc > 2)
    {
        if (strncmp(argv[2], "-l", max_read) == 0)
        {
            if (argc <= 3)
            {
                fprintf(stderr, "No specified line size, exiting\n");
                exit(1);
            }

            line_size = strtol(argv[3], NULL, 10);
            if (line_size == 0)
            {
                fprintf(stderr, "%s is not an integer value or is invalid\n", argv[3]);
                exit(1);
            } 
            else if (line_size < 0)
            {
                fprintf(stderr, "%s is a negative number and is not a valid line size\n", argv[3]);
                exit(1);
            }
        }
    }

    if (!hexdump(file, (size_t)line_size, mode, col, extended))
    {
        fprintf(stderr, "Error dumping hexadecimal data\n");
        exit(1);
    }

    fclose(file);
    return 0;
}
