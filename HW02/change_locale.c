#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

int main() {
    // Get the current locale without changing it
    char *current_locale = setlocale(LC_ALL, NULL);

    if (current_locale == NULL) {
        printf("Failed to retrieve the current locale.\n");
        return 1;
    }

    printf("Current locale: %s\n", current_locale);
    current_locale = setlocale(LC_ALL, ""); // use system default locale
    printf("Current locale: %s\n", current_locale);
    int code_point = 0x2605; // Unicode code point for 'ё'
    wint_t r = putwchar(code_point);

    if (r == WEOF) {
        printf("Failed to print the character.\n");
        return 1;
    }

    return 0;
}
