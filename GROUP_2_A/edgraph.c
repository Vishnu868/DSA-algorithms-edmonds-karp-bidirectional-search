#include "edgraph.h"

Graph* createGraph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->nodes = (Node*)malloc(vertices * sizeof(Node));

    //adjacency matrix
    graph->adjMatrix = (int**)malloc(vertices * sizeof(int*));
    for (int i = 0; i < vertices; i++) {
        graph->adjMatrix[i] = (int*)calloc(vertices, sizeof(int));
        graph->nodes[i].vertex = i;
        graph->nodes[i].url = NULL;
        graph->nodes[i].edges = NULL;
        graph->nodes[i].numEdges = 0;
    }

    return graph;
}

void addEdge(Graph* graph, int src, int dest, int weight) {
    // Add to edges list
    Edge* edge = (Edge*)malloc(sizeof(Edge));
    edge->dest = dest;
    edge->weight = weight;

    graph->nodes[src].edges = (Edge*)realloc(graph->nodes[src].edges, 
        (graph->nodes[src].numEdges + 1) * sizeof(Edge));
    graph->nodes[src].edges[graph->nodes[src].numEdges++] = *edge;
    free(edge);

    //adjacency matrix
    graph->adjMatrix[src][dest] = weight;
}

int findVertexByUrl(Graph* graph, const char* url) {
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url && strcmp(graph->nodes[i].url, url) == 0) {
            return i;
        }
    }
    return -1;
}

void processUrlFile(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Cannot open file '%s'\n", filename);
        return;
    }

    char fromUrl[MAX_URL_LENGTH], toUrl[MAX_URL_LENGTH];
    int weight;
    int nextVertex = 0;
    char line[MAX_URL_LENGTH * 2 + 50];  // Buffer for whole line
    
    printf("Starting to read file '%s'...\n", filename);
    
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';
        
        char* fromUrlToken = strtok(line, ",");
        char* toUrlToken = strtok(NULL, ",");
        char* weightToken = strtok(NULL, ",");
        
        if (fromUrlToken && toUrlToken && weightToken) {
            weight = atoi(weightToken);
            
            strncpy(fromUrl, fromUrlToken, MAX_URL_LENGTH - 1);
            strncpy(toUrl, toUrlToken, MAX_URL_LENGTH - 1);
            fromUrl[MAX_URL_LENGTH - 1] = '\0';
            toUrl[MAX_URL_LENGTH - 1] = '\0';
            
            if (strlen(fromUrl) >= MAX_URL_LENGTH || strlen(toUrl) >= MAX_URL_LENGTH) {
                printf("Warning: URL too long, skipping line\n");
                continue;
            }
            
            if (weight < 0) {
                printf("Warning: Negative weight found, using absolute value\n");
                weight = abs(weight);
            }
            
            int fromIndex = findVertexByUrl(graph, fromUrl);
            if (fromIndex == -1) {
                if (nextVertex >= MAX_VERTICES) {
                    printf("Error: Maximum number of vertices reached\n");
                    break;
                }
                fromIndex = nextVertex++;
                graph->nodes[fromIndex].url = strdup(fromUrl);
                printf("Created new vertex for %s at index %d\n", fromUrl, fromIndex);
            }

            int toIndex = findVertexByUrl(graph, toUrl);
            if (toIndex == -1) {
                if (nextVertex >= MAX_VERTICES) {
                    printf("Error: Maximum number of vertices reached\n");
                    break;
                }
                toIndex = nextVertex++;
                graph->nodes[toIndex].url = strdup(toUrl);
                printf("Created new vertex for %s at index %d\n", toUrl, toIndex);
            }

            addEdge(graph, fromIndex, toIndex, weight);
            printf("Added edge: %s -> %s (Weight: %d)\n", fromUrl, toUrl, weight);
        } else {
            printf("Warning: Invalid line format: %s\n", line);
        }
    }
    
    if (ferror(file)) {
        printf("Error: Failed to read file\n");
    } else if (feof(file)) {
        printf("Finished reading file. Processed %d vertices\n", nextVertex);
    }
    
    fclose(file);
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

// Implements BFS
int bfs(Graph* graph, int* parent, int source, int sink) {
    // Initialize visited array
    int* visited = (int*)calloc(graph->numVertices, sizeof(int));
    if (!visited) return 0;
    
    //queue for BFS
    int* queue = (int*)malloc(graph->numVertices * sizeof(int));
    if (!queue) {
        free(visited);
        return 0;
    }
    int front = 0, rear = 0;
    
    // Start BFS
    visited[source] = 1;
    queue[rear++] = source;
    parent[source] = -1;
    
    //BFS loop
    while (front < rear) {
        int u = queue[front++];
        
        //adjacent vertices
        for (int v = 0; v < graph->numVertices; v++) {
            // If not visited and has capacity
            if (!visited[v] && graph->adjMatrix[u][v] > 0) {
                visited[v] = 1;
                queue[rear++] = v;
                parent[v] = u;
                
                if (v == sink) {
                    free(visited);
                    free(queue);
                    return 1;
                }
            }
        }
    }
    
    free(visited);
    free(queue);
    return 0;
}

int edmondsKarp(Graph* graph, const char* source_url, const char* sink_url) {
    int source = findVertexByUrl(graph, source_url);
    int sink = findVertexByUrl(graph, sink_url);
    
    if (source == -1 || sink == -1) {
        printf("Error: Source or sink URL not found in graph\n");
        return -1;
    }
    
    //residual graph
    int** residual = (int**)malloc(graph->numVertices * sizeof(int*));
    if (!residual) return -1;
    
    for (int i = 0; i < graph->numVertices; i++) {
        residual[i] = (int*)malloc(graph->numVertices * sizeof(int));
        if (!residual[i]) {
            for (int j = 0; j < i; j++) free(residual[j]);
            free(residual);
            return -1;
        }
        memcpy(residual[i], graph->adjMatrix[i], graph->numVertices * sizeof(int));
    }
    
    // Parent array for storing BFS path
    int* parent = (int*)malloc(graph->numVertices * sizeof(int));
    if (!parent) {
        for (int i = 0; i < graph->numVertices; i++) free(residual[i]);
        free(residual);
        return -1;
    }
    
    int max_flow = 0;
    
    while (bfs(graph, parent, source, sink)) {
        // Find minimum residual capacity along the path
        int path_flow = INT_MAX;
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            path_flow = min(path_flow, graph->adjMatrix[u][v]);
        }
        
        // Update residual capacities and reverse edges
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];
            graph->adjMatrix[u][v] -= path_flow;
            graph->adjMatrix[v][u] += path_flow;
        }
        
        max_flow += path_flow;
        
        printf("Found augmenting path with flow: %d\n", path_flow);
        printf("Path: %s", graph->nodes[sink].url);
        for (int v = sink; v != source; v = parent[v]) {
            printf(" <- %s", graph->nodes[parent[v]].url);
        }
        printf("\nCurrent max flow: %d\n\n", max_flow);
    }
    
    for (int i = 0; i < graph->numVertices; i++) {
        free(residual[i]);
    }
    free(residual);
    free(parent);
    
    return max_flow;
}

void printWeightedEdgeList(Graph* graph) {
    printf("\n=== Weighted Edge List ===\n");
    printf("From URL -> To URL (Weight)\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {  // Only print if URL exists
            for (int j = 0; j < graph->nodes[i].numEdges; j++) {
                int destIndex = graph->nodes[i].edges[j].dest;
                printf("%-30s -> %-30s (Weight: %d)\n",
                    graph->nodes[i].url,
                    graph->nodes[destIndex].url,
                    graph->nodes[i].edges[j].weight);
            }
        }
    }
}
void printAdjacencyList(Graph* graph) {
    printf("\n=== Weighted Adjacency List ===\n");
    printf("URL -> [Destination URLs]\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {  // Only print if URL exists
            printf("%-30s ->", graph->nodes[i].url);
            for (int j = 0; j < graph->nodes[i].numEdges; j++) {
                int destIndex = graph->nodes[i].edges[j].dest;
                printf(" %s (%d)", 
                    graph->nodes[destIndex].url,
                    graph->nodes[i].edges[j].weight);
            }
            printf("\n");
        }
    }
}

void printAdjacencyMatrix(Graph* graph) {
    printf("\n=== Weighted Adjacency Matrix ===\n");
    
    printf("%5s", "");
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {
            printf("%5d ", i);
        }
    }
    printf("\n%5s", "");
    
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {
            printf("------");
        }
    }
    printf("\n");
    
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {
            printf("[%3d] ", i);
            for (int j = 0; j < graph->numVertices; j++) {
                if (graph->nodes[j].url) {
                    printf("%5d ", graph->adjMatrix[i][j]);
                }
            }
            printf("\n");
        }
    }
}
void writeGraphToDot(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "digraph G {\n");
    for (int i = 0; i < graph->numVertices; i++) {
        for (int j = 0; j < graph->nodes[i].numEdges; j++) {
            fprintf(file, "  \"%s\" -> \"%s\" [label=\"%d\"];\n",
                graph->nodes[i].url,
                graph->nodes[graph->nodes[i].edges[j].dest].url,
                graph->nodes[i].edges[j].weight);
        }
    }
    fprintf(file, "}\n");
    fclose(file);
}

void freeGraph(Graph* graph) {
    for (int i = 0; i < graph->numVertices; i++) {
        free(graph->nodes[i].url);
        free(graph->nodes[i].edges);
        free(graph->adjMatrix[i]);
    }
    free(graph->adjMatrix);
    free(graph->nodes);
    free(graph);
}