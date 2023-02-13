// COMP2521 Assignment 2

// Written by: Leo Shi z5364321
// Date: November 2021

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// used Graph.h + Graph.c from LAB exercises
#include "Graph.h"
#include "utility.h"

char **getCollection();
int collectionNodes(char **url_index);
Graph getGraph(char **url_index);
double weightIn(Graph g, int row, int col);
double nodeIn(Graph g, int a);
double weightOut(Graph g, int row, int col);
double nodeOut(Graph g, int a);
double** createWeightMatrix(Graph g, char **url_index, int nodes);
double calculateExistsIn(Graph g, char **url_index, double **weight_matrix, double PR_previous_array[], int p_i);
int outDegree(Graph g, int a);
int indexFinder(double number, double PR_current_array[], int nodes);


int main(int argc, char *argv[]) {

    // argv[1] = damping factor
    double damping_factor = atof(argv[1]);
    // argv[2] = diffPR (sum of PageRank differences)
    double diffPR = atof(argv[2]);
    // argv[3] = maxIterations(maximum number of iterations)
    int maxIterations = atoi(argv[3]);

    // create a graph that includes n number of nodes
    char **url_index = getCollection();
    
    // create the empty graph to hold url data
    Graph url_graph = getGraph(url_index);
    int nodes = url_graph->nV;
    // create our weight matrix (stores W_in * W_out values)
    double** weight_matrix = createWeightMatrix(url_graph, url_index, nodes);

    // create 2 arrays to store the current and previous iterations of pagerank
    // set all weight values of iteration 0 to 1/N.
    double PR_previous_array[nodes];
    for (int i = 0; i < nodes; i++) {
        PR_previous_array[i] = 0;
    }
    double PR_current_array[nodes];
    for (int i = 0; i < nodes; i++) {
        PR_current_array[i] = 1.0 / nodes;
    }

    // PAGERANK FORMULA
    double diff = diffPR;
    for (int iterations = 0; iterations < maxIterations && diff >= diffPR; iterations++) {
        // we have not reached the end of our iterations
        // make PR_current = PR_previous
        for (int i = 0; i < url_graph->nV; i++) {
            PR_previous_array[i] = PR_current_array[i];
        }
        for (int p_i = 0; p_i < url_graph->nV; p_i++) {
            PR_current_array[p_i] = ((1 - damping_factor) / url_graph->nV)
                + (damping_factor * calculateExistsIn(url_graph, url_index, weight_matrix, PR_previous_array, p_i));
        }
        // calculate diff
        diff = 0;
        for (int i = 0; i < url_graph->nV; i++) {
            double a = fabs(PR_current_array[i] - PR_previous_array[i]);
            diff = diff + a;
        }
    }

    // recreate the PR_current_array
    double temp[nodes];
    for (int i = 0; i < nodes; i++) {
        temp[i] = PR_current_array[i];
    }
    // sort the temp file by using a stable sort, in descending pagerank
    bubbleSort(temp, 0, nodes - 1);
    
    //open output file and proceed to print out result
    FILE *output_fp;
    if((output_fp = fopen("pagerankList.txt","w")) == NULL) {
        fprintf(stderr,"Cannot open the file\n");
        exit(1);
    }

    for (int i = nodes - 1; i >= 0; i--) {
        int index = indexFinder(temp[i], PR_current_array, nodes);
        fprintf(output_fp, "%s, ", url_index[index]);
        fprintf(output_fp, "%d, ", outDegree(url_graph, index));
        fprintf(output_fp, "%.7lf\n", temp[i]);
    }

    // free allocated memory
    destroyArray(weight_matrix);
    free_tokens(url_index);
    GraphFree(url_graph);
    return 0;
}

char **getCollection() {
    FILE *fp;
    if((fp = fopen("collection.txt","r")) == NULL) {
        fprintf(stderr,"Cannot open the file\n");
        exit(1);
    }
    // Read the data set and get the number of url
    char url[100];
    int n = 0;
    char url_2[2048] = {0};
    while (fscanf(fp, "%s", url) != EOF) {
        strcat(url_2, url);
        strcat(url_2, "/");
        n++;
    }
    char **url_index = tokenize(url_2, "/");
    return url_index;
}

int collectionNodes(char **collection) {
    int nodes = 0;
    for (int i = 0; strcmp(collection[i], "\0") != 0; i++) {
        nodes++;
    }
    return nodes;
}

// find the url index (integer between 0 and n)
int getUrlIndex(char **url_index, char file_url[]) {
    int index;
    for (index = 0; strcmp(url_index[index], "\0") != 0; index++) {
        if (strcmp(url_index[index], file_url) == 0) {
            break;
        }
    }
    return index;
}

Graph getGraph(char **collection) {
    Graph g = GraphNew(collectionNodes(collection));
    // open the respective url.txt file to see the edges of the node
    for (int i = 0; strcmp(collection[i], "\0") != 0; i++) {
        FILE *url_fp;
        char filename[100];
        strcpy(filename, collection[i]);
        strcat(filename, ".txt");
        if((url_fp = fopen(filename,"r")) == NULL) {
            fprintf(stderr,"Cannot open the file\n");
            exit(1);
        }
        // collect the number of nodes to determine the weight of each edge
        rewind(url_fp);

        // insert edges into the graph
        char file_url[100];
        int word_counter = 0;
        while (fscanf(url_fp, "%s", file_url) != EOF) {
            if (strcmp(file_url, "#end") == 0) {
                break;
            } else if (word_counter < 2) {
                // dont scan
            } else if (strcmp(file_url, collection[i]) != 0) {
                int v = getUrlIndex(collection, collection[i]);
                int w = getUrlIndex(collection, file_url);
                if (GraphIsAdjacent(g, v, w) != 0.0) {
                    // edge already exists, we want to ignore it
                } else {
                    Edge e;
                    e.v = v;
                    e.w = w;
                    e.weight = 1;
                    GraphInsertEdge(g, e);
                }
            }
            word_counter++;
        }
    }
    return g;
}

double weightIn(Graph g, int row, int col) {
    // if edge = AB, AB = [row][col]
    // weight in = In(B) / In(all nodes pointing at A)
    double numerator = nodeIn(g, col);
    double denominator = 0;
    // all nodes pointing at A --> loop through A's col
    for (int i = 0; i < g->nV; i++) {
        if (g->edges[row][i] != 0) {
            // has an edge
            denominator = denominator + nodeIn(g, i);
        }
    }
    double weight = 0.0;
    if (numerator != 0 && denominator != 0) {
        weight = numerator / denominator;
    } else {
        weight = 0.5;
    }
    return weight;
}

double nodeIn(Graph g, int a) {
    double x = 0;
    // we can find the number of in edges by searching through
    // the column of the node
    for (int i = 0; i < g->nV; i++) {
        if (g->edges[i][a] == 1) {
            // has an edge
            x++;
        }
    }
    return x;
}

double weightOut(Graph g, int row, int col) {
    // if edge = AB,
    // weight out = Out(B) / Out(all nodes pointing at A)
    double numerator = nodeOut(g, col);
    double denominator = 0;
    // all nodes pointing at A --> loop through A's col
    for (int i = 0; i < g->nV; i++) {
        if (g->edges[row][i] != 0) {
            // has an edge
            denominator = denominator + nodeOut(g, i);
        }
    }
    double weight = 0.0;
    if (numerator != 0 && denominator != 0) {
        weight = numerator / denominator;
    } else {
        weight = 0.5;
    }
    return weight;
}

double nodeOut(Graph g, int a) {
    double x = 0;
    // we can find the number of out edges by searching through
    // the row of the node
    int trigger = 0;
    for (int i = 0; i < g->nV; i++) {
        if (g->edges[a][i] == 1) {
            // has an edge
            x++;
            trigger = 1;
        }
    }
    if (trigger == 0) {
        x = 0.5;
    }
    return x;
}

double** createWeightMatrix(Graph g, char **url_index, int nodes) {
    // create our weightIn and weightOut matrixes
    double** weightInMatrix = createArray(nodes, nodes);
    double** weightOutMatrix = createArray(nodes, nodes);

    // loop through our edge graph and calculate the weight of each edge
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            if (g->edges[i][j] != 0) {
                // there is an edge, now calculate the weight
                weightInMatrix[i][j] = weightIn(g, i, j);
                weightOutMatrix[i][j] = weightOut(g, i, j);
            } else {
                weightInMatrix[i][j] = 0;
                weightOutMatrix[i][j] = 0;
            }
        }
    }
    // we can calculate our final weight matrix by multiplying in & out together
    double** weightMatrix = createArray(nodes, nodes);
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            weightMatrix[i][j] = weightInMatrix[i][j] * weightOutMatrix[i][j];
        }
    }
    destroyArray(weightInMatrix);
    destroyArray(weightOutMatrix);
    return weightMatrix;
}

double calculateExistsIn(Graph g, char **url_index, double **weight_matrix, double PR_previous_array[], int p_i) {
    // find all elements which points at url_index[p_i]
    // we can find the number of in edges by searching through
    // the column of the node 
    double value = 0;
    for (int i = 0; i < g->nV; i++) {
        if (g->edges[i][p_i] == 1) {
            // has an edge
            value = value + (PR_previous_array[i] * weight_matrix[i][p_i]);
        }
    }
    return value;
}

int outDegree(Graph g, int a) {
    int x = 0;
    // we can find the number of out edges by searching through
    // the row of the node
    for (int i = 0; i < g->nV; i++) {
        if (g->edges[a][i] == 1) {
            // has an edge
            x++;
        }
    }
    return x;
}

int indexFinder(double number, double PR_current_array[], int nodes) {
    int i;
    for (i = 0; i < nodes; i++) {
        if (PR_current_array[i] == number) {
            break;
        }
    }
    return i;
}