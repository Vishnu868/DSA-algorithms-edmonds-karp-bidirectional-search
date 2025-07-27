#ifndef BDGRAPH_H
#define BDGRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_VERTICES 1000
#define MAX_URL_LENGTH 256

typedef struct Edge {
    int dest;
    int weight;
} Edge;

typedef struct Node {
    int vertex;
    char* url;
    Edge* edges;
    int numEdges;
} Node;

typedef struct Graph {
    Node* nodes;
    int** adjMatrix;
    int numVertices;
} Graph;

typedef struct SearchState {
    int* visited;
    int* parent;
    int* distance;
    int* queue;
    int front;
    int rear;
} SearchState;

typedef struct Path {
    int* path;
    int length;
} Path;

Graph* createGraph(int vertices);
void addEdge(Graph* graph, int src, int dest, int weight);
int findVertexByUrl(Graph* graph, const char* url);
void processUrlFile(Graph* graph, const char* filename);
void freeGraph(Graph* graph);

void printWeightedEdgeList(Graph* graph);
void printAdjacencyList(Graph* graph);
void printAdjacencyMatrix(Graph* graph);
void writeGraphToDot(Graph* graph, const char* filename);

SearchState* createSearchState(int vertices);
void freeSearchState(SearchState* state);
void bfsStep(Graph* graph, SearchState* state, int vertex, int forward);
Path* reconstructPath(SearchState* forward, SearchState* backward, 
                     int source, int target, int intersection);
Path* bidirectionalSearch(Graph* graph, const char* source_url, const char* target_url);
void printPathDetails(Graph* graph, Path* path);
void visualizeBidirectionalPath(Graph* graph, Path* path, const char* filename);

void freePath(Path* path);

#endif