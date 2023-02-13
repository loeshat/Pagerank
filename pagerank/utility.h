// COMP2521 Assignment 2

// Written by: Leo Shi z5364321
// Date: November 2021


// add your own #includes, #defines, typedefs, structs and
// function prototypes here

// will return 1 if char *a begins with char *b
bool StartsWith(const char *a, const char *b);

// enter a string with words separated with the *sep, returns a 2d array string 
char **tokenize (char *str, char *sep);

// free memory allocated with tokenize
void free_tokens(char **tokens);

// dynamically allocate memory to create a 2d array with dimensions of m x n
double** createArray(int m, int n);

// free memory allocated with createArray
void destroyArray(double** arr);

// Bubblesort - taken from Course Lecture code
int less(double x, double y);
void swap(double a[], int i, int j);
void bubbleSort(double a[], int lo, int hi);

