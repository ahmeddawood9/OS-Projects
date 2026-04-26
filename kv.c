#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    int key;
    char *value;
    struct Node *next;
} Node;

Node *head = NULL;

void put(int key, char *value) {
    Node *curr = head;
    while (curr != NULL) {
        if (curr->key == key) {
            free(curr->value);
            curr->value = strdup(value);
            return;
        }
        curr = curr->next;
    }
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("malloc");
        exit(1);
    }
    new_node->key = key;
    new_node->value = strdup(value);
    new_node->next = head;
    head = new_node;
}

char* get(int key) {
    Node *curr = head;
    while (curr != NULL) {
        if (curr->key == key) {
            return curr->value;
        }
        curr = curr->next;
    }
    return NULL;
}

int delete(int key) {
    Node *curr = head;
    Node *prev = NULL;
    while (curr != NULL) {
        if (curr->key == key) {
            if (prev == NULL) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr->value);
            free(curr);
            return 0;
        }
        prev = curr;
        curr = curr->next;
    }
    return -1;
}

void clear() {
    Node *curr = head;
    while (curr != NULL) {
        Node *next = curr->next;
        free(curr->value);
        free(curr);
        curr = next;
    }
    head = NULL;
}

void print_all() {
    Node *curr = head;
    while (curr != NULL) {
        printf("%d,%s\n", curr->key, curr->value);
        curr = curr->next;
    }
}

void load() {
    FILE *fp = fopen("database.txt", "r");
    if (fp == NULL) {
        return;
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, fp) != -1) {
        char *ptr = line;
        char *key_str = strsep(&ptr, ",");
        char *value = strsep(&ptr, "\n");
        if (key_str != NULL && value != NULL) {
            put(atoi(key_str), value);
        }
    }
    free(line);
    fclose(fp);
}

int main(int argc, char *argv[]) {
    load();
    if (argc < 2) {
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        char *arg = strdup(argv[i]);
        char *cmd = strsep(&arg, ",");
        
        if (cmd == NULL) {
            printf("bad command\n");
            continue;
        }

        if (strcmp(cmd, "p") == 0) {
            char *key_str = strsep(&arg, ",");
            char *value = strsep(&arg, ",");
            if (key_str == NULL || value == NULL) {
                printf("bad command\n");
                continue;
            }
            int key = atoi(key_str);
            put(key, value);
        } else if (strcmp(cmd, "g") == 0) {
            char *key_str = strsep(&arg, ",");
            if (key_str == NULL) {
                printf("bad command\n");
                continue;
            }
            int key = atoi(key_str);
            char *value = get(key);
            if (value != NULL) {
                printf("%d,%s\n", key, value);
            } else {
                printf("%d not found\n", key);
            }
        } else if (strcmp(cmd, "d") == 0) {
            char *key_str = strsep(&arg, ",");
            if (key_str == NULL) {
                printf("bad command\n");
                continue;
            }
            int key = atoi(key_str);
            if (delete(key) != 0) {
                printf("%d not found\n", key);
            }
        } else if (strcmp(cmd, "c") == 0) {
            clear();
        } else if (strcmp(cmd, "a") == 0) {
            print_all();
        } else {
            printf("bad command\n");
        }
        free(arg);
    }

    return 0;
}
