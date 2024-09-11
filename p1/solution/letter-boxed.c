#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>
#include <sys/types.h>

#define MIN_BOARD_SIZE 3
#define MIN_DICT_SIZE 10
#define MAX_CHARS 26

int main(int argc, char* argv[]) {
    // return code of the function
    int rc = 0;

    if (argc < 3) {
        printf("Usage: %s <boardfile> <dictfile>\n", argv[0]);
        rc = 1;
        exit(rc);
    }

    const char *boardFile = argv[1];
    FILE *board = fopen(boardFile, "r");
    if (board == NULL) {
        perror("fopen: board file not found!\n");
        rc = 1;
        exit(rc);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    size_t totalUniqueLetters = 0;
    int *letterCount = calloc(MAX_CHARS, sizeof(int));
    int *lettersUsed = calloc(MAX_CHARS, sizeof(int));
    int *letterSideMap = malloc(MAX_CHARS * sizeof(int));
    char **sides = malloc(MIN_BOARD_SIZE * sizeof(char*));
    size_t boardSize = MIN_BOARD_SIZE;
    size_t sideCount = 0;

    // how to make sure "line" memory is freed if getline() errors out?
    while ((read = getline(&line, &len, board)) != -1) {
        for (size_t i = 0; i < strlen(line); i++) {
            if (line[i] == '\n') continue;

            // check characters in the board are unique and lowercase only
            int letterIdx = (int) line[i] - 97;
            if (!islower(line[i]) || !(letterIdx >= 0 && letterIdx < MAX_CHARS && letterCount[letterIdx] == 0)) {
                printf("Invalid board\n");
                rc = 1;
                goto cleanup_board_and_exit;
            }
            letterCount[letterIdx] += 1;
            // if first occurence of letter
            if (letterCount[letterIdx] == 1) {
                lettersUsed[letterIdx] = 1;
                totalUniqueLetters += 1;
            }
            letterSideMap[letterIdx] = sideCount + 1;
        }

        if (sideCount >= boardSize) {
            boardSize *= 2;
            sides = realloc(sides, boardSize * sizeof(char*));
        }
        sides[sideCount] = malloc(strlen(line) + 1);
        strcpy(sides[sideCount], line);
        sideCount += 1;
    }

    if (sideCount < MIN_BOARD_SIZE) {
        printf("Invalid board\n");
        rc = 1;
        goto cleanup_board_and_exit;
    }
    free(line);
    fclose(board);

    const char *dictFile = argv[2];
    FILE *dict = fopen(dictFile, "r");
    if (dict == NULL) {
        perror("fopen: dictionary file not found!\n");
        rc = 1;
        goto cleanup_board_and_exit;
    }

    char *word = NULL;
    len = 0;
    char **words = malloc(MIN_DICT_SIZE * sizeof(char*));
    size_t dictSize = MIN_DICT_SIZE;
    size_t wordCount = 0;
    while ((read = getline(&word, &len, dict)) != -1) {
        size_t wordSize = strlen(word);

        if (wordCount >= dictSize) {
            dictSize *= 2;
            words = realloc(words, dictSize * sizeof(char*));
        }
        words[wordCount] = malloc(wordSize + 1);
        strcpy(words[wordCount], word);
        wordCount += 1;
    }
    free(word);
    fclose(dict);

    char *input = NULL;
    char lastLetter = '\0';
    len = 0;
    while ((read = getline(&input, &len, stdin)) != -1) {
        if (read == 1) continue;

        // 1. check for invalid letters
        for (size_t i = 0; i < strlen(input); i++) {
            char l = input[i];
            if (l == '\n') continue;

            int letterIdx = (int) l - 97;
            if (!islower(l) || (islower(l) && letterCount[letterIdx] == 0)) {
                printf("Used a letter not present on the board\n");
                rc = 0;
                goto cleanup_and_exit;
            }
        }

        // 2. check word continuity
        if (lastLetter != '\0' && input[0] == lastLetter) {
            printf("First letter of word does not match last letter of previous word\n");
            rc = 0;
            goto cleanup_and_exit;
        }
        lastLetter = input[strlen(input) - 1];


        // 3. check for consecutive same-side letters
        char prevChar = '\0';
        for (size_t i = 0; i < strlen(input); i++) {
            char l = input[i];
            if (l == '\n') continue;

            int prevCharIdx = (int) prevChar - 97;
            int letterIdx = (int) l - 97;
            if (prevChar != '\0' && letterSideMap[prevCharIdx] == letterSideMap[letterIdx]) {
                printf("Same-side letter used consecutively\n");
                rc = 0;
                goto cleanup_and_exit;
            }
            prevChar = l;
        }

        // 4. check in dictionary
        int found = 0;
        for (size_t i = 0; i < wordCount; i++) {
            if (strcmp(words[i], input)) {
                found = 1;
                // decrement count for every used letter
                for (size_t j = 0; j < strlen(input); j++) {
                    if (input[j] == '\n') continue;

                    int letterIdx = (int) input[j] - 97;
                    if (lettersUsed[letterIdx] == 1) totalUniqueLetters--;
                    lettersUsed[letterIdx] = 0;
                    // if all letters have been used, break
                    if (totalUniqueLetters == 0) goto success;
                }
            }
        }
        if (found == 0) {
            printf("Word not found in dictionary\n");
            rc = 0;
            goto cleanup_and_exit;
        }
    }

    // 5. check for unused letters
    if (totalUniqueLetters != 0) {
        printf("Not all letters used\n");
        rc = 0;
        goto cleanup_and_exit;
    }

success:
    printf("Correct\n");
    rc = 0;
    goto cleanup_and_exit;

cleanup_board_and_exit:
    free(letterCount);
    free(lettersUsed);
    free(letterSideMap);
    free(line);
    free(sides);
    return rc;

cleanup_and_exit:
    free(letterCount);
    free(lettersUsed);
    free(letterSideMap);
    free(sides);
    free(words);
    return rc;
}
