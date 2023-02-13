
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"


bool StartsWith(const char *a, const char *b) {
   if(strncmp(a, b, strlen(b)) == 0) {
       return 1;
   } else {
       return 0;
   }
}

char **tokenize (char *str, char *sep) {
    size_t n_tokens = 0;
    char **tokens = NULL;

    while (*str != '\0') {
        str += strspn (str, sep);
        if (*str == '\0')
            break;
        size_t len = strcspn (str, sep);
        char *tok = strndup (str, len);
        assert (tok != NULL);
        str += len;

        tokens = realloc (tokens, ++n_tokens * sizeof *tokens);
        tokens[n_tokens - 1] = tok;
        
    }
    tokens = realloc (tokens, ++n_tokens * sizeof *tokens);
    tokens[n_tokens - 1] = "\0";
    return tokens;
}

// Free an array of strings as returned by `tokenize'.
void free_tokens(char **tokens) {
    for (int i = 0; strcmp(tokens[i], "\0") != 0; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

double** createArray(int m, int n) {
    double* values = calloc(m * n, sizeof(double));
    double** rows = malloc(m * sizeof(double*));
    for (int i = 0; i < m; i++) {
        rows[i] = values + i * n;
    }
    return rows;
}


void destroyArray(double** array) {
    free(*array);
    free(array);
}

int less(double x, double y) {
	if (x < y) {
        return 1;
    } else {
        return 0;
    }
}

void swap(double a[], int i, int j) {
	double tmp = a[i]; 
    a[i] = a[j]; 
    a[j] = tmp;
}

// bubble sort
void bubbleSort(double a[], int lo, int hi) {
   int i, j, nswaps;
   for (i = lo; i < hi; i++) {
      nswaps = 0;
      for (j = hi; j > i; j--) {
         if (less(a[j], a[j - 1])) {
            swap(a, j, j-1);
            nswaps++;
         }
      }
      if (nswaps == 0) break;
   }
}