#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILEPATH_MAX_SIZE 128llu
#define LINE_SIZE 16llu

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

bool hexdump(FILE* file, unsigned char mode)
{
    size_t filesize = 0llu; // file size in bytes

    fseek(file, 0, SEEK_END); // find end of file
    filesize = ftell(file);
    fseek(file, 0, SEEK_SET); // back to beggining of file

    size_t num_lines = (size_t)(filesize / LINE_SIZE) + 1;

    printf("[mode %u]: ", mode);
    for (size_t i = 0; i < LINE_SIZE; i++)
    {
        printf("%02llX ", i);
    }

    printf("\n");

    switch (mode) 
    {
        case MODE_BINARY:
        {
            for (size_t i = 0; i < num_lines; i += 1)
            {
                byte line[LINE_SIZE] = { 0x00 };
                fread(line, sizeof(byte), LINE_SIZE, file); 

                printf("%08llX: ", i * LINE_SIZE);

                for (size_t j = 0; j < LINE_SIZE; j++)
                {
                    printf("%02hhX ", line[j]);
                }

                printf("\n");
            }
            break;
        }

        case MODE_ASCII:
        {
            for (size_t i = 0; i < num_lines; i += 1)
            {
                byte line[LINE_SIZE] = { 0x00 };
                fread(line, sizeof(byte), LINE_SIZE, file); 

                printf("%08llX: ", i * LINE_SIZE);

                for (size_t j = 0; j < LINE_SIZE; j++)
                {
                    char c = (char)line[j];
                    // ignore non printable characters
                    if (c < 31)
                    {
                        printf(" . ");
                    }
                    else 
                    {
                        printf(" %c ", line[j]);
                    }
                }
                printf("\n");
            }
            break;
        }

        default:
        {
            fprintf(stderr, "Invalid mode %u\n", mode);
            return false;
        }
    }

    return true;   
}

void usage(void)
{
    printf("Usage:\n");
    printf("    hxdump <file/filepath> <arguments> <flags>\n");
    printf("\n");
    printf("    arguments:\n");
    printf("     -h     prints usage (what you are currently reading)\n");
    printf("\n");
    printf("    flags:\n");
    printf("     --binary   open files in binary mode (mode 0)\n");
    printf("     --ascii    open files in ascii mode  (mode 1)\n");

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

    unsigned char mode = 0;

    const size_t max_read = 12;
    if (argc == 3)
    {
        if (strncmp(argv[2], "--binary", max_read) == 0)
        {
            mode = MODE_BINARY;
        }
        else if (strncmp(argv[2], "--ascii", max_read) == 0)
        {
            mode = MODE_ASCII;
        }
        else
        {
            fprintf(stderr, "Invalid flag %s\n", argv[2]);
            exit(1);
        }
    }

    if (!hexdump(file, mode))
    {
        fprintf(stderr, "Error dumping hexadecimal data\n");
        exit(1);
    }

    fclose(file);
    return 0;
}
