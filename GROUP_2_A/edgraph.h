#ifndef GRAPH_H
#define GRAPH_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_URL_LENGTH 256
#define MAX_VERTICES 100

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
    int numVertices;
    int** adjMatrix;
} Graph;

Graph* createGraph(int vertices);
void addEdge(Graph* graph, int src, int dest, int weight);
void printAdjacencyList(Graph* graph);
void printWeightedEdgeList(Graph* graph);
void printAdjacencyMatrix(Graph* graph);
int findVertexByUrl(Graph* graph, const char* url);
void processUrlFile(Graph* graph, const char* filename);
void writeGraphToDot(Graph* graph, const char* filename);
void freeGraph(Graph* graph);

int min(int a, int b);
int bfs(Graph* graph, int* parent, int source, int sink);
int edmondsKarp(Graph* graph, const char* source_url, const char* sink_url);

#endif