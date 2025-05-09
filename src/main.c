#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#define MAX_FILE_SIZE 10485760  // 10MB


// array of pointers
static char *banned_words[] = {"jane_doe", "admin", "guest"};

void process_buffer(char *buffer) {
    //printf("%s", buffer);

    // because strtok modifies its original value we should strdup it
    char *buffer_copy = strdup(buffer);

    if (buffer_copy == NULL) {
        perror("strdup");
        return; // because we want it to close the file from earlier
    }

    char *token = strtok(buffer_copy, " \t\n-.");
    while (token != NULL) {
        int redacted = 0;
        size_t num_banned = sizeof(banned_words) / sizeof(banned_words[0]);

        for (size_t i = 0; i < num_banned; i++) {
            if (strcmp(banned_words[i], token) == 0) {
                printf("[REDACTED]");
                redacted = 1;
                break;
            }
        }

        if (!redacted) {
            printf("%s", token);
        }

        printf(" ");  // add a space between words

        token = strtok(NULL, " \t\n-.");
    }


    free(buffer_copy);
}

int main(int argc, char **argv) {

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }


    char *buffer = malloc(MAX_FILE_SIZE);
    if (buffer == NULL) {
        perror("malloc");
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    size_t bytesRead = fread(buffer, 1, MAX_FILE_SIZE - 1, fp);
    if (bytesRead == 0 && ferror(fp)) {
        perror("fread");
        free(buffer);
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    buffer[bytesRead] = '\0';

    process_buffer(buffer);

    free(buffer);
    fclose(fp);
    return 0;
}
