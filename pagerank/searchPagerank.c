// COMP2521 Assignment 2

// Written by: Leo Shi
// Date:

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utility.h"

#define MAX_URL 100
#define LINE_MAX 1000


int urlLocation(char* url, char** url_index);
char** sortOnPagerank();
void sortByPagerank(char **order, int temp[], char **pagerank_index, int counter);


int main(int argc, char *argv[]) {
    // read from stdin for webpage names
    FILE *fp;
    if((fp = fopen("invertedIndex.txt","r")) == NULL) {
        // check if the file pointer exists or not
        fprintf(stderr,"Cannot open the file\n");
        exit(1);
    }
    char total_url[MAX_URL * LINE_MAX];
    char input[MAX_URL];
    for (int i = 1; i < argc; i++) {
        // scan through fp to find the webpage name
        while (fscanf(fp, "%s", input) != EOF) {
            if (strcmp(input, argv[i]) == 0) {
                // the scanned in word is one of the webpage names
                char url[LINE_MAX];
                // scan in all the words until the new line
                fscanf(fp, "%[^\n]", url);
                if (i == 1) {
                    strcpy(total_url, url);
                } else {
                    strcat(total_url, url);
                }
            }
        }
        rewind(fp);
    }
    // we now have a string that contains all of the url links
    // we need to now count them and rank them based on how frequently
    // they appear in the list

    // sort url_index in terms of url names
    // a 2d string array where each url is one of the entries
    char **url_index = tokenize(total_url, " ");

    
    char order_temp[10000];
    strcpy(order_temp, "/");
    // counter to keep track of unique urls found within array
    int counter = 0;
    for (int i = 0; strcmp(url_index[i], "\0") != 0; i++) {
        int first_time_found = urlLocation(url_index[i], url_index);
        if (first_time_found == i) {
            // this is the first time we have encountered this url
            // url not found in list, add it at position counter
            strcat(order_temp, url_index[i]);
            strcat(order_temp, "/");
            counter++;
        }
    }
    // order is a 2D array string that contains the order of which each unique
    // url first appears
    char** order = tokenize(order_temp, "/");

    // declare array to store the frequency
    int url_frequency[MAX_URL] = {0};
    // loop through url_index and + 1 to the frequency every time a url appears
    for (int i = 0; strcmp(url_index[i], "\0") != 0; i++) {
        for (int j = 0; j < counter; j++) {
            if (strcmp(order[j], url_index[i]) == 0) {
                url_frequency[j]++;
            }
        }
    }

    // create an array that stores urls based on their pagerank (descending order)
    char **pagerank_index = sortOnPagerank();

    // print out relevant url's by frequency, then pagerank
    for (int i = counter; i > 0; i--) {
        int counter2 = 0;
        int temp[MAX_URL] = {42};
        for (int j = 0; url_frequency[j] != 0; j++) {
            if (url_frequency[j] == i) {
                temp[counter2] = j;
                counter2++;
            }
        }
        sortByPagerank(order, temp, pagerank_index, counter2);
    }


    // free all allocated memory
    free_tokens(pagerank_index);
    free_tokens(order);
    free_tokens(url_index);
    return 0;
}

int urlLocation(char* url, char** url_index) {
    // identify when was the first time a certain url pops up in the url_index
    // used for identifying if the url scanned in is unique, or has already been seen before
    for (int i = 0; strcmp(url_index[i], "\0") != 0; i++) {
        if (strcmp(url_index[i], url) == 0) {
            return i;
        }
    }
    return 0;
}

char** sortOnPagerank() {
    // open the pagerank file and create a string array that sorts the
    // url's by pagerank (descending order)
    FILE *fp_pagerank;
    if((fp_pagerank = fopen("pagerankList.txt","r")) == NULL) {
        fprintf(stderr,"Cannot open the file\n");
        exit(1);
    }
    char input[1000] = {0};
    char c;
    strcpy(input, "/");
    int i = 0;
    int tracker = 0;
    while ((c = fgetc(fp_pagerank)) != EOF) {
        if (c == ',') {
            input[i] = '/';
            i++;
            tracker++;
        } else if (c == '\n') {
            tracker = 0;
        } else if (tracker == 0) {
            input[i] = c;
            i++;
        }
    }
    char **pagerank = tokenize(input, "/");
    return pagerank;
}

void sortByPagerank(char **order, int temp[], char **pagerank_index, int counter) {
    // we are given an array (temp) filled with url's that had the same frequency.
    // we want to print these url's based on their pagerank order
    for (int i = 0; strcmp(pagerank_index[i], "\0") != 0; i++) {
        for (int j = 0; j < counter; j++) {
            if (strcmp(order[temp[j]], pagerank_index[i]) == 0) {
                // found the one with the highest pagerank
                // print out the url
                printf("%s\n", order[temp[j]]);
            }
        }
    }
}
