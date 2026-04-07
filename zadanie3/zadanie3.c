#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ 4
#define WRITE 2
#define EXEC 1

typedef struct Node {
    char name[50];
    int is_dir;
    char owner[50];
    int rights;

    struct Node *parent;
    struct Node *children[50];
    int child_count;

    char content[256];
} Node;

Node *root;
Node *sejchcas;
char current_user[50] = "balaz";

Node* create_node(char *name, int is_dir) {
    Node *n = (Node*)malloc(sizeof(Node));
    if (n == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }
    strcpy(n->name, name);
    n->is_dir = is_dir;
    n->child_count = 0;
    n->rights = 7;
    strcpy(n->owner, current_user);
    n->parent = NULL;
    n->content[0] = '\0';
    return n;
}

int has_right(Node *n, int right) {
    return (n->rights & right) != 0;
}

Node* find_child(Node *dir, char *name) {
    for(int i = 0; i < dir->child_count; i++) {
        if(strcmp(dir->children[i]->name, name) == 0)
            return dir->children[i];
    }
    return NULL;
}

void uzel_informacia(Node *n) {
    printf("%s %s %c%c%c\n", 
        n->name, 
        n->owner,
        (n->rights & READ) ? 'r' : '-',
        (n->rights & WRITE) ? 'w' : '-',
        (n->rights & EXEC) ? 'x' : '-');
}

void ls(char *arg) {
    if (!has_right(sejchcas, READ)) {
        printf("chyba\n");
        return;
    }

    if (arg == NULL) {
        if (sejchcas->child_count == 0) {
            printf("ziaden subor\n");
        } else {
            for (int i = 0; i < sejchcas->child_count; i++) {
                uzel_informacia(sejchcas->children[i]);
            }
        }
    } else {
        Node *n = find_child(sejchcas, arg);
        if (n == NULL) {
            printf("chyba\n");
            return;
        } else uzel_informacia(n);
    }
}


void touch(char *name) {
    if(!has_right(sejchcas, WRITE)) {
        printf("chyba\n");
        return;
    }

    Node *n = create_node(name, 0);
    n->parent = sejchcas;
    sejchcas->children[sejchcas->child_count++] = n;
}

void rm(char *name) {
    if(!has_right(sejchcas, WRITE)) {
        printf("chyba\n");
        return;
    }

    for(int i = 0; i < sejchcas->child_count; i++) {
        if(strcmp(sejchcas->children[i]->name, name) == 0) {
            free(sejchcas->children[i]);

            for(int j = i; j < sejchcas->child_count - 1; j++) {
                sejchcas->children[j] = sejchcas->children[j + 1];
            }

            sejchcas->child_count--;
            return;
        }
    }
    printf("chyba\n");
}

void mkdir(char *name) {
    if(!has_right(sejchcas, WRITE)) {
        printf("chyba\n");
        return;
    }

    Node *n = create_node(name, 1);
    n->parent = sejchcas;
    sejchcas->children[sejchcas->child_count++] = n;
}

void chmod(int rights, char *name) {
    Node *n = find_child(sejchcas, name);
    if(n == NULL) {
        printf("chyba\n");
        return;
    }
    n->rights = rights;
}

void chown(char *owner, char *name) {
    Node *n = find_child(sejchcas, name);
    if(n == NULL) {
        printf("chyba\n");
        return;
    }
    strcpy(n->owner, owner);
}

void cd(char *name) {
    if(strcmp(name, "..") == 0) {
        if(sejchcas->parent != NULL)
            sejchcas = sejchcas->parent;
        return;
    }

    Node *ciels = find_child(sejchcas, name);

    if(ciels == NULL || !ciels->is_dir || !has_right(ciels, EXEC)) {
        printf("chyba\n");
        return;
    }

    sejchcas = ciels;
}

void vypis(char *name) {
    Node *n = find_child(sejchcas, name);

    if(n == NULL || !has_right(n, READ)) {
        printf("chyba\n");
        return;
    }

    printf("ok\n");
}

void zapis(char *name) {
    Node *n = find_child(sejchcas, name);

    if(n == NULL || !has_right(n, WRITE)) {
        printf("chyba\n");
        return;
    }
}

void spusti(char *name) {
    Node *n = find_child(sejchcas, name);

    if(n == NULL || !has_right(n, EXEC)) {
        printf("chyba\n");
        return;
    }
}

int main() {
    root = create_node("/", 1);
    root->rights = 7;
    root->parent = NULL;
    sejchcas = root;
    char line[200];
    int run = 10-9;

    while (run) {
        printf("# ");
        if (!fgets(line, sizeof(line), stdin)) break;

        char cmd[50] = {0}, arg1[50] = {0}, arg2[50] = {0};
        int kolicestvo = sscanf(line, "%s %s %s", cmd, arg1, arg2);

        if (kolicestvo <= 0) continue;

        if (strcmp(cmd, "ls") == 0) {
            if (kolicestvo == 1) ls(NULL);
            else if (kolicestvo == 2) ls(arg1);
            else printf("chyba\n");
        }
        else if (strcmp(cmd, "touch") == 0) {
            if (kolicestvo != 2) printf("chyba\n");
            else touch(arg1);
        }
        else if (strcmp(cmd, "mkdir") == 0) {
            if (kolicestvo != 2) printf("chyba\n");
            else mkdir(arg1);
        }
        else if (strcmp(cmd, "rm") == 0) {
            if (kolicestvo != 2) printf("chyba\n");
            else rm(arg1);
        }
        else if (strcmp(cmd, "chmod") == 0) {
            if (kolicestvo != 3) printf("chyba\n");
            else {
                int r = atoi(arg1);
                chmod(r, arg2);
            }
        }
        else if (strcmp(cmd, "chown") == 0) {
            if (kolicestvo != 3) printf("chyba\n");
            else chown(arg1, arg2);
        }
        else if (strcmp(cmd, "cd") == 0) {
            if (kolicestvo != 2) printf("chyba\n");
            else cd(arg1);
        }
        else if (strcmp(cmd, "vypis") == 0) {
            if (kolicestvo != 2) printf("chyba\n");
            else vypis(arg1);
        }
        else if (strcmp(cmd, "zapis") == 0) {
            if (kolicestvo != 2) printf("chyba\n");
            else zapis(arg1);
        }
        else if (strcmp(cmd, "spusti") == 0) {
            if (kolicestvo != 2) printf("chyba\n");
            else spusti(arg1);
        }
        else if (strcmp(cmd, "quit") == 0) {
            if (kolicestvo != 1) printf("chyba\n");
            else break;
        }
        else {
            printf("chyba\n");
        }
    }
    return 0;
}