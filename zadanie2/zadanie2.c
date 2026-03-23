#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int shifting(const char *heslo) {
    int shift = 0;

    for (int i = 0; i < strlen(heslo); i++) {
        shift += heslo[i] * (i + 3) + (i * i) * (i + 1);
    }

    return shift % 256;
}

unsigned long mojhash(const char *str) {
    unsigned long h = 1469598103934665603UL;
    int shift = shifting(str);
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = str[i];
        c = c + shift + i;
        h ^= c;
        h ^= (i * 31);
        h *= 1099511628211UL;
        h = (h << 7) | (h >> (sizeof(unsigned long)*8 - 7));
        h += (c * (i + 1));
    }
    return h;
}


int authenticate(char lines[][1024], int count, char *meno, unsigned long zadany_hash, char *kluc) {
    for (int i = 0; i < count; i++) {
        char line[1024];
        strcpy(line, lines[i]);
        char *user = strtok(line, ":");
        char *hash_str = strtok(NULL, ":");
        char *keys = strtok(NULL, "\n");
        if (!user || !hash_str || !keys) continue;

        if (strcmp(user, meno) == 0) {
            unsigned long stored_hash = strtoul(hash_str, NULL, 10);
            if (stored_hash != zadany_hash) return 0;
            char novij_kluc[1024] = "";
            char *token = strtok(keys, ",");
            int poisk_klucha = 0;

            while (token) {
                if (strcmp(token, kluc) == 0 && !poisk_klucha) {
                    poisk_klucha = 1;
                } else {
                    if (strlen(novij_kluc) > 0)
                        strcat(novij_kluc, ",");
                    strcat(novij_kluc, token);
                }
                token = strtok(NULL, ",");
            }

            if (!poisk_klucha) return 0;
            memset(lines[i], 0, sizeof(lines[i]));
            sprintf(lines[i], "%s:%lu:%s\n", user, stored_hash, novij_kluc);
            return 1;
        }
    }
    return 0;
}

int main() {
    char meno[80];
    char heslo[80];
    char kluc[80];

    printf("meno: ");
    scanf("%s", meno);

    printf("heslo: ");
    scanf("%s", heslo);

    printf("overovaci kluc: ");
    scanf("%s", kluc);

    unsigned long zadany_hash = mojhash(heslo);

    FILE *f = fopen("hesla.csv", "r");
    if (!f) {
        printf("chyba\n");
        return 0;
    }
    char lines[100][1024];
    int count = 0;
    while (fgets(lines[count], 1024, f)) {
        count++;
    }
    fclose(f);

    int success = authenticate(lines, count, meno, zadany_hash, kluc);

    if (success) {
        FILE *fw = fopen("hesla.csv", "w");
        if (!fw) {
            printf("chyba\n");
            return 0;
        }
        for (int i = 0; i < count; i++) {
            fputs(lines[i], fw);
        }
        fclose(fw);
        printf("ok\n");
    } else {printf("chyba\n");}

    return 0;
}
