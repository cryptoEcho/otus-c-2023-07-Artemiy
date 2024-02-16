#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>

#define PROG_NAME "rus2utf"
#define ARG_ENCODE_LONG "--encode"
#define ARG_ENCODE_SHORT "-e"
#define ARG_OUTPUTFILE "-of"
#define ARG_HELP "--help"
//#define INPUT_FILE "koi8.txt"
#define MAXLEN_FILENAME 25
#define ARGC_OF_SPECIFIED 6

#define S_KOI8 "koi8-r"
#define S_CP1251 "cp-1251"
#define S_ISO8859 "iso-8859-5"
#define H_KOI8 0x001
#define H_CP1251 0x010
#define H_ISO8859 0x100

// processing cli arguments
void cli_args(int argc, char** argv, char** pinput_filename, char** poutput_filename);
wchar_t convert_to_utf8(unsigned char symbol);

unsigned short koi8r_to_unicode[128] = {
    0x2500,0x2502,0x250c,0x2510,0x2514,0x2518,0x251c,0x2524,
    0x252c,0x2534,0x253c,0x2580,0x2584,0x2588,0x258c,0x2590,
    0x2591,0x2592,0x2593,0x2320,0x25a0,0x2219,0x221a,0x2248,
    0x2264,0x2265,0x00a0,0x2321,0x00b0,0x00b2,0x00b7,0x00f7,
    0x2550,0x2551,0x2552,0x0451,0x2553,0x2554,0x2555,0x2556,
    0x2557,0x2558,0x2559,0x255a,0x255b,0x255c,0x255d,0x255e,
    0x255f,0x2560,0x2561,0x0401,0x2562,0x2563,0x2564,0x2565,
    0x2566,0x2567,0x2568,0x2569,0x256a,0x256b,0x256c,0x00a9,
    0x044e,0x0430,0x0431,0x0446,0x0434,0x0435,0x0444,0x0433,
    0x0445,0x0438,0x0439,0x043a,0x043b,0x043c,0x043d,0x043e,
    0x043f,0x044f,0x0440,0x0441,0x0442,0x0443,0x0436,0x0432,
    0x044c,0x044b,0x0437,0x0448,0x044d,0x0449,0x0447,0x044a,
    0x042e,0x0410,0x0411,0x0426,0x0414,0x0415,0x0424,0x0413,
    0x0425,0x0418,0x0419,0x041a,0x041b,0x041c,0x041d,0x041e,
    0x041f,0x042f,0x0420,0x0421,0x0422,0x0423,0x0416,0x0412,
    0x042c,0x042b,0x0417,0x0428,0x042d,0x0429,0x0427,0x042a
};

unsigned short cp1251_to_unicode[128] = {
    0x0402, 0x0403, 0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021,
    0x20AC, 0x2030, 0x0409, 0x2039, 0x040A, 0x040C, 0x040B, 0x040F,
    0x0452, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
    0x003F, 0x2122, 0x0459, 0x203A, 0x045A, 0x045C, 0x045B, 0x045F,
    0x00A0, 0x040E, 0x045E, 0x0408, 0x00A4, 0x0490, 0x00A6, 0x00A7,
    0x0401, 0x00A9, 0x0404, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x0407,
    0x00B0, 0x00B1, 0x0406, 0x0456, 0x0491, 0x00B5, 0x00B6, 0x00B7,
    0x0451, 0x2116, 0x0454, 0x00BB, 0x0458, 0x0405, 0x0455, 0x0457,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F
};

unsigned short iso88595_to_unicode[128] = {
    // control characters
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    // 0xFFD - no Unicode equivalent
    0x0098, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
    // printable characters
    0x00A0, 0x0401, 0x0402, 0x0403, 0x0404, 0x0405, 0x0406, 0x0407,
    0x0408, 0x0409, 0x040A, 0x040B, 0x040C, 0x00AD, 0x040E, 0x040F,
    0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417,
    0x0418, 0x0419, 0x041A, 0x041B, 0x041C, 0x041D, 0x041E, 0x041F,
    0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427,
    0x0428, 0x0429, 0x042A, 0x042B, 0x042C, 0x042D, 0x042E, 0x042F,
    0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437,
    0x0438, 0x0439, 0x043A, 0x043B, 0x043C, 0x043D, 0x043E, 0x043F,
    0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447,
    0x0448, 0x0449, 0x044A, 0x044B, 0x044C, 0x044D, 0x044E, 0x044F,
    0x2116, 0x0451, 0x0452, 0x0453, 0x0454, 0x0455, 0x0456, 0x0457,
    0x0458, 0x0459, 0x045A, 0x045B, 0x045C, 0x00A7, 0x045E, 0x045F
};
int encoding = 0x000;


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
