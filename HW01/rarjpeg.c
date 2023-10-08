#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

#define FILENAME_SIZE_SHIFT 26
#define FILENAME_SHIFT 30

void argc_mismatch(void);
void print_help(void);
int get_filename_from_archive(size_t ii, const unsigned char buffer[], unsigned char** filename);
void print_archive(size_t file_size, char buffer[]);
char* args_process(const int argc, const char* argv[]);


int main(int argc, char* argv[])
{
    /* filename input */
    unsigned char* filename = args_process(argc, argv);
        
    // open file
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("File opening failed");
        exit(EXIT_FAILURE);
    }

    // read file
    fseek(fp, 0L, SEEK_END);
    unsigned int file_size = ftell(fp);
    rewind(fp);

    // allocate buffer and read the whole file
    unsigned char* buffer = (unsigned char*) malloc(file_size * sizeof(char));
    if (buffer == NULL) {
        fprintf(stderr, "Error during malloc\n");
        exit(EXIT_FAILURE);
    }
    const size_t ret_code = fread(buffer, sizeof(char), file_size, fp);
    // handle errors
    if (ret_code == file_size) {
        // printf("file was read successfully. File size is %d bytes\n", file_size);
    }
    else {
        if (feof(fp))
            fprintf(stderr, "Error reading test.bin: unexpected end of file\n");
        else if (ferror(fp))
            perror("Error reading test.bin");
    }

    /* file analysis */
    const unsigned char ZIP_SIGN[2] = {0x50, 0x4b};
    char israrjpeg = 0;
    for (size_t i = 0; i < file_size; i++) {
        /*  ZIP SIGNATURES:
            0x50 0x4b 0x01 0x02 - central dir
            0x50 0x4b 0x03 0x04 - local file
            0x50 0x4b 0x05 0x06 - end of central dir
        */
        if ( (buffer[i] == ZIP_SIGN[0]) && (buffer[i+1] == ZIP_SIGN[1]) ) {
            if ( (buffer[i+2]) < 7 && (buffer[i+3] < 7) ) {
                israrjpeg = 1;
                break;
            }
        }
    }

    /* print results of checking file */
    if (israrjpeg) {
        print_archive(file_size, buffer);
    }
    else {
        printf("This file does not include an archive\n");
    }


    exit(EXIT_SUCCESS);

}

int get_filename_from_archive(size_t ii, const unsigned char buffer[], unsigned char** filename)
{
    size_t filename_size = buffer[ii+FILENAME_SIZE_SHIFT + 1] << 8 | buffer[ii+FILENAME_SIZE_SHIFT];
    *filename = (unsigned char*) malloc(filename_size * sizeof(unsigned char));
    if (*filename == NULL) {
        fprintf(stderr, "Error during malloc\n");
        free(buffer);
        buffer = NULL;
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < filename_size; i++) {
        (*filename)[i] = buffer[ii+FILENAME_SHIFT + i];
    }


    return filename_size;
}

void print_help(void)
{
    fprintf(stdout, "Usage: rarjpeg FILE\n"
                    "Determines if the file is a rarjpeg. "
                    "If so, prints the contents of the archive.\n"
            );
}

void argc_mismatch(void)
{
    fprintf(stderr, "rarjpeg: missing operand\n"
                    "Try 'rarjpeg --help' for more information\n"
            );
}

void print_archive(size_t file_size, char buffer[])
{
    unsigned char* filename;
    const unsigned char FILE_SIGN[4] = {0x50, 0x4b, 0x03, 0x04};
    char fsign_found = 0;
    // bruteforce
    for (size_t i = 0; i < file_size; i++) {
        if (buffer[i] == 0x50) {
            fsign_found = 1;
            for (size_t k = 1; k < sizeof(FILE_SIGN); k++) {
                if (buffer[i+k] != FILE_SIGN[k]) {
                    fsign_found = 0;
                    break;
                }
            }
            if (fsign_found) {
                size_t filename_size = get_filename_from_archive(i, buffer, &filename);
                // print name of file from archive
                for (size_t j = 0; j < filename_size; j++) {
                    printf("%c", filename[j]);
                }
                printf("\n");
            }
        }
    }
}

char* args_process(const int argc, const char* argv[])
{
    if (argc < 2) {
        argc_mismatch();
        exit(EXIT_FAILURE);
    }
    else if (!strcmp(argv[1], "--help")) {
        print_help();
        exit(EXIT_SUCCESS);
    }
    
    return argv[1];
}