#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include "utils.h"

// processing cli arguments
void cli_args(int argc, char** argv, char** pinput_filename, char** poutput_filename);
wchar_t convert_to_utf8(unsigned char symbol);



int main(int argc, char** argv)
{
    // set locale to system default (C.UTF-8)
    // output will be wrong if it is not set
    setlocale(LC_CTYPE, ""); 

    //cli_args: --encoding (or -e) CP-1251, KOI8-R, ISO-8859-5 (non case-sensitive); -if "input_file.txt"; -of "output_file.txt"
    // if no output_file is specified, stdout will be used
    char *input_filename, *output_filename;
    cli_args(argc, argv, &input_filename, &output_filename);
    
    // open files
    FILE* input_file = fopen(input_filename, "r");
    if (input_file == NULL) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    
    FILE* output_file;
    if (argc == ARGC_OF_SPECIFIED) {
        output_file = fopen(output_filename, "w");
        if (output_file == NULL) {
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }
    }

    // read input file and write in output file in parallel
    unsigned char byte; 
    while (fread(&byte, 1, 1, input_file)) {
        // printf("%c", byte);
        wchar_t utf8_symbol = convert_to_utf8(byte);
        // printf("%u\t%hx\t", koi82unicode[byte-128], koi82unicode[byte-128]);
        // if output file is specified write to it
        if (argc == ARGC_OF_SPECIFIED) {
            fwprintf(output_file, L"%lc", utf8_symbol);
        } else {
        // if not, use stdout
        fwprintf(stdout, L"%lc", utf8_symbol);
        }
    }

    free(input_filename);
    fclose(input_file);
    if (argc == ARGC_OF_SPECIFIED) {
        free(output_filename);
        fclose(output_file);
    }
    return 0;

    //fclose(output_file);
}

/* convert symbol from cyrrilic encoding KOI8-R, CP-1251, ISO-8859-5 to UTF-8 */
wchar_t convert_to_utf8(unsigned char symbol)
{
    if (symbol < 0x80) {
        return (wchar_t)symbol;
    }
    switch(encoding) {
         case H_KOI8: return koi8r_to_unicode[symbol-128]; break;
         case H_CP1251: return cp1251_to_unicode[symbol-128]; break;
         case H_ISO8859: return iso88595_to_unicode[symbol-128]; break;
         default: return -1;
    }
#if 0
    if (!strcmp(encoding, "koi8-r")) {
        return koi8r_to_unicode[symbol-128];
    }
    else if (!strcmp(encoding, "cp-1251")) {
        return cp1251_to_unicode[symbol-128];
    }
    else if (!strcmp(encoding, "iso-8859-5")) {
        return iso88595_to_unicode[symbol-128];
    }
#endif

}

// process input arguments from cli
void cli_args(int argc, char** argv, char** pinput_filename, char** poutput_filename)
{
    // if no arguments is specified, an error with hint returned
    if (argc == 1) {
        fprintf(stderr, 
                "Error: no argument is specified.\n"
                "Try "PROG_NAME" --help\n");
        exit(EXIT_FAILURE);
    }

    // --help argument was specified
    if (argc == 2 && !strcmp(ARG_HELP, argv[1])) {
        fprintf(stdout,
                PROG_NAME" - commandline converter from cyrrilic encoding to UTF8 [version 0.0.1]\n"
                "\nUsage:\t"PROG_NAME" --encode [encoding] [INPUT_FILE]\n"
                "\t"PROG_NAME" --encode [encoding] [INPUT_FILE] -of [OUTPUT_FILE]\n"
                "Options:\n"
                "  -e, --encode=encoding\tspecify input file encoding\n"
                "\t\t\tpossible values: 'koi8-r', 'cp-1251', 'iso-8859-5'.\n"
                "  -of\t\t\tspecify output file (optional)\n"
                "\nInput file must be specified right after it's encoding.\n"
                );
        exit(EXIT_SUCCESS);
    }

    // if number of arguments is incorrect return an error
    if (argc != 4 && argc != ARGC_OF_SPECIFIED) {
        fprintf(stderr,
                "Error: incorrect number of arguments.\n"
                "Try "PROG_NAME" --help\n");
        exit(EXIT_FAILURE);
    }


    // check that aruments '-e' '--encoding' is specified
    if (strcmp(ARG_ENCODE_LONG, argv[1]) && strcmp(ARG_ENCODE_SHORT, argv[1])) {
        fprintf(stderr,
                "Error. Wrong argument.\n"
                "Try "PROG_NAME" --help\n");
        exit(EXIT_FAILURE);
    }
    
    // encoding selection
    if (!strcmp(S_KOI8, argv[2])) {
        encoding |= H_KOI8;
    } else if (!strcmp(S_CP1251, argv[2])) {
        encoding |= H_CP1251;
    } else if (!strcmp(S_ISO8859, argv[2])) {
        encoding |= H_ISO8859;
    } else {
        fprintf(stderr,
                "Error. Wrong argument.\n"
                "Try "PROG_NAME" --help\n");
        exit(EXIT_FAILURE);
    }

    // input filename
    char* input_filename = malloc( strnlen(argv[3]+1, MAXLEN_FILENAME) );
    if (input_filename == NULL) {
        perror("input filename");
        abort();
    }
    strcpy(input_filename, argv[3]);
   
    // if output file is specified
    if (argc == ARGC_OF_SPECIFIED) {
        // then -of must be specified as 4th argument
        if (strcmp(ARG_OUTPUTFILE, argv[4])) {
           fprintf(stderr,
                   "Error: wrong argument.\n"
                   "Try "PROG_NAME" --help\n");
           free(input_filename);
           exit(EXIT_FAILURE);
        }
        // output filename
        char* output_filename = malloc( strnlen(argv[5]+1, MAXLEN_FILENAME) );
        if (output_filename == NULL) {
            perror("output filename");
            free(input_filename);
            abort();
        }
        strcpy(output_filename, argv[5]);

        *poutput_filename = output_filename;
    }
    *pinput_filename = input_filename;
}
