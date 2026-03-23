#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int shifting(const char *heslo) {
    int shift = 0;

    for (int i = 0; i < strlen(heslo); i++) {
        shift += heslo[i] * (i + 3) + (i * i) * (i + 1);
    }

    return shift % 256;
}

void caesar(const char *vstupni_subor, const char *vystupni_subor, const char *heslo, int shift) {

    FILE *input = fopen(vstupni_subor, "rb");
    FILE *output = fopen(vystupni_subor, "wb");

    if (input == NULL || output == NULL) {
        printf("chyba");
        if (input) fclose(input);
        if (output) fclose(output);
        exit(1);
    }

    int c;
    int pos = 0;

    while ((c = fgetc(input)) != EOF) {
        int dynamic_shift = (shifting(heslo) + (pos * (int)strlen(heslo))) % 256;

        if (shift == 1) {
            fputc((c + dynamic_shift) % 256, output);
        } 
        else {
            fputc((c - dynamic_shift + 256) % 256, output);
        }

        pos++;
    }

    fclose(input);
    fclose(output);
}

int main(int argc, char *argv[]) {

    char *input = NULL;
    char *output = NULL;
    char *heslo = NULL;

    bool s_flag = false;
    bool d_flag = false;

    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "-s") == 0) {
            s_flag = true;
        }

        else if (strcmp(argv[i], "-d") == 0) {
            d_flag = true;
        }

        else if (strcmp(argv[i], "-p") == 0 && i + 1 < argc) {
            heslo = argv[++i];
        }

        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            input = argv[++i];
        }

        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output = argv[++i];
        }

        else {
            printf("chyba");
            return 1;
        }
    }

    if (s_flag == d_flag || input == NULL || output == NULL || heslo == NULL) {
        printf("chyba");
        return 1;
    }

    int mode = s_flag ? 1 : 0;

    caesar(input, output, heslo, mode);

    return 0;
}