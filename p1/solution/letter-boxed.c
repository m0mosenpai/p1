#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h>

#define MIN_BOARD_SIZE 3
#define MIN_DICT_SIZE 10

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <boardfile> <dictfile>\n", argv[0]);
        return 1;
    }

    const char *boardFile = argv[1];
    FILE *board = fopen(boardFile, "r");
    if (board == NULL) {
        // use perror when a function writes to errno
        // use fprintf(stderr, ...) otherwise
        perror("fopen: board file not found!\n");
        return 1;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char **sides = malloc(MIN_BOARD_SIZE * sizeof(char*));
    size_t boardSize = MIN_BOARD_SIZE;
    size_t sideCount = 0;
    // how to make sure "line" memory is freed if getline() errors out?
    while ((read = getline(&line, &len, board)) != -1) {
        if (sideCount >= boardSize) {
            boardSize *= 2;
            sides = realloc(sides, boardSize * sizeof(char*));
        }
        sides[sideCount] = malloc(strlen(line) + 1);

        // validate line only contains lowercase alphabets
        for (size_t i = 0; i < strlen(line); i++) {
            if (line[i] == '\n') continue;
            if (!islower(line[i])) {
                fprintf(stderr, "Invalid Board\n");
                free(line);
                fclose(board);
                return 1;
            }
        }
        strcpy(sides[sideCount], line);
        sideCount += 1;
    }
    free(line);
    fclose(board);

    if (sideCount < MIN_BOARD_SIZE) {
        fprintf(stderr, "Invalid Board\n");
        return 1;
    }

    /*for (size_t i = 0; i < sideCount; i++) {*/
    /*    printf("%s", sides[i]);*/
    /*}*/

    const char *dictFile = argv[2];
    FILE *dict = fopen(dictFile, "r");
    if (dict == NULL) {
        perror("fopen: dictionary file not found!\n");
        return 1;
    }

    char *word = NULL;
    len = 0;
    char **words = malloc(MIN_DICT_SIZE * sizeof(char*));
    size_t dictSize = MIN_DICT_SIZE;
    size_t wordCount = 0;
    while ((read = getline(&word, &len, dict)) != -1) {
        if (wordCount >= dictSize) {
            dictSize *= 2;
            words = realloc(words, dictSize * sizeof(char*));
        }
        words[wordCount] = malloc(strlen(line) + 1);
        strcpy(words[wordCount], line);
        wordCount += 1;
    }
    free(word);
    fclose(dict);

    /*for (size_t i = 0; i < wordCount; i++) {*/
    /*    printf("%s", words[i]);*/
    /*}*/

    // TO-DO: Main Letter Boxed Logic

    return 0;
}
