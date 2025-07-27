#include "bdgraph.h"

Graph* createGraph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->nodes = (Node*)malloc(vertices * sizeof(Node));

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
    Edge* edge = (Edge*)malloc(sizeof(Edge));
    edge->dest = dest;
    edge->weight = weight;

    graph->nodes[src].edges = (Edge*)realloc(graph->nodes[src].edges, 
        (graph->nodes[src].numEdges + 1) * sizeof(Edge));
    graph->nodes[src].edges[graph->nodes[src].numEdges++] = *edge;
    free(edge);

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
    char line[MAX_URL_LENGTH * 2 + 50];

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
        }
    }
    
    fclose(file);
}

void freeGraph(Graph* graph) {
    if (!graph) return;
    
    for (int i = 0; i < graph->numVertices; i++) {
        free(graph->adjMatrix[i]);
        free(graph->nodes[i].url);
        free(graph->nodes[i].edges);
    }
    free(graph->adjMatrix);
    free(graph->nodes);
    free(graph);
}

void printWeightedEdgeList(Graph* graph) {
    printf("\n=== Weighted Edge List ===\n");
    printf("From URL -> To URL (Weight)\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {
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
        if (graph->nodes[i].url) {
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

SearchState* createSearchState(int vertices) {
    SearchState* state = (SearchState*)malloc(sizeof(SearchState));
    state->visited = (int*)calloc(vertices, sizeof(int));
    state->parent = (int*)malloc(vertices * sizeof(int));
    state->distance = (int*)malloc(vertices * sizeof(int));
    state->queue = (int*)malloc(vertices * sizeof(int));
    state->front = 0;
    state->rear = 0;
    
    for (int i = 0; i < vertices; i++) {
        state->distance[i] = INT_MAX;
        state->parent[i] = -1;
    }
    
    return state;
}

void freeSearchState(SearchState* state) {
    free(state->visited);
    free(state->parent);
    free(state->distance);
    free(state->queue);
    free(state);
}

void bfsStep(Graph* graph, SearchState* state, int vertex, int forward) {
    for (int i = 0; i < graph->numVertices; i++) {
        int hasEdge = forward ? graph->adjMatrix[vertex][i] : graph->adjMatrix[i][vertex];
        if (hasEdge && !state->visited[i]) {
            state->queue[state->rear++] = i;
            state->visited[i] = 1;
            state->parent[i] = vertex;
            state->distance[i] = state->distance[vertex] + 1;
        }
    }
}

Path* reconstructPath(SearchState* forward, SearchState* backward, 
                     int __attribute__((unused)) source, 
                     int __attribute__((unused)) target, 
                     int intersection) {
    int length = forward->distance[intersection] + backward->distance[intersection] + 1;
    Path* path = (Path*)malloc(sizeof(Path));
    path->path = (int*)malloc(length * sizeof(int));
    path->length = length;
    
    int current = intersection;
    int idx = forward->distance[intersection];
    
    while (current != -1) {
        path->path[idx--] = current;
        current = forward->parent[current];
    }
    
    current = backward->parent[intersection];
    idx = forward->distance[intersection] + 1;
    while (current != -1) {
        path->path[idx++] = current;
        current = backward->parent[current];
    }
    
    return path;
}

Path* bidirectionalSearch(Graph* graph, const char* source_url, const char* target_url) {
    int source = findVertexByUrl(graph, source_url);
    int target = findVertexByUrl(graph, target_url);
    
    if (source == -1 || target == -1) {
        printf("Error: Source or target URL not found in graph\n");
        return NULL;
    }
    
    printf("\n=== Starting Bidirectional Search ===\n");
    printf("Source URL: %s (Node %d)\n", source_url, source);
    printf("Target URL: %s (Node %d)\n", target_url, target);
    
    SearchState* forward = createSearchState(graph->numVertices);
    SearchState* backward = createSearchState(graph->numVertices);
    
    forward->queue[forward->rear++] = source;
    forward->visited[source] = 1;
    forward->distance[source] = 0;
    
    backward->queue[backward->rear++] = target;
    backward->visited[target] = 1;
    backward->distance[target] = 0;
    
    int intersection = -1;
    int min_path_length = INT_MAX;
    int iterations = 0;
    
    printf("\nSearch Progress:\n");
    printf("----------------\n");
    
    while (forward->front < forward->rear && backward->front < backward->rear) {
        iterations++;
        printf("\nIteration %d:\n", iterations);
        
        // Forward search step
        int current_forward = forward->queue[forward->front++];
        printf("Forward frontier: %s\n", graph->nodes[current_forward].url);
        bfsStep(graph, forward, current_forward, 1);
        
        if (backward->visited[current_forward]) {
            int path_length = forward->distance[current_forward] + 
                            backward->distance[current_forward];
            if (path_length < min_path_length) {
                min_path_length = path_length;
                intersection = current_forward;
                printf("Found potential meeting point at: %s (distance: %d)\n", 
                       graph->nodes[intersection].url, path_length);
            }
        }
        
        int current_backward = backward->queue[backward->front++];
        printf("Backward frontier: %s\n", graph->nodes[current_backward].url);
        bfsStep(graph, backward, current_backward, 0);
        
        if (forward->visited[current_backward]) {
            int path_length = forward->distance[current_backward] + 
                            backward->distance[current_backward];
            if (path_length < min_path_length) {
                min_path_length = path_length;
                intersection = current_backward;
                printf("Found potential meeting point at: %s (distance: %d)\n", 
                       graph->nodes[intersection].url, path_length);
            }
        }
    }
    
    Path* result = NULL;
    if (intersection != -1) {
        result = reconstructPath(forward, backward, source, target, intersection);
        printf("\n=== Bidirectional Search Complete ===\n");
        printf("Meeting point: %s\n", graph->nodes[intersection].url);
        printf("Total iterations: %d\n", iterations);
        printf("Path found: ");
        for (int i = 0; i < result->length; i++) {
            printf("%s", graph->nodes[result->path[i]].url);
            if (i < result->length - 1) printf(" -> ");
        }
        printf("\nNumber of hops: %d\n", result->length - 1);
    } else {
        printf("\n=== Bidirectional Search Complete ===\n");
        printf("No path found between %s and %s\n", source_url, target_url);
        printf("Total iterations: %d\n", iterations);
    }
    
    freeSearchState(forward);
    freeSearchState(backward);
    
    return result;
}



void printPathDetails(Graph* graph, Path* path) {
    if (!path || path->length <= 1) return;
    
    printf("\n=== Path Details ===\n");
    printf("Step\tFrom URL\tTo URL\tWeight\n");
    printf("----------------------------------------\n");
    
    int total_weight = 0;
    for (int i = 0; i < path->length - 1; i++) {
        int from = path->path[i];
        int to = path->path[i + 1];
        int weight = graph->adjMatrix[from][to];
        total_weight += weight;
        
        printf("%d.\t%s\t->\t%s\t%d\n",
               i + 1,
               graph->nodes[from].url,
               graph->nodes[to].url,
               weight);
    }
    
    printf("\nTotal path weight: %d\n", total_weight);
}

void writeGraphToDot(Graph* graph, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "digraph G {\n");
    fprintf(file, "  node [shape=box];\n");
    fprintf(file, "  rankdir=LR;\n");
    
    // Add all nodes first
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {
            fprintf(file, "  \"%s\" [label=\"%s\"];\n",
                graph->nodes[i].url,
                graph->nodes[i].url);
        }
    }
    
    // Add all edges
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

void visualizeBidirectionalPath(Graph* graph, Path* path, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    fprintf(file, "digraph BidirectionalPath {\n");
    fprintf(file, "  node [shape=box];\n");
    fprintf(file, "  rankdir=LR;\n");
    
    fprintf(file, "  node [style=filled,fillcolor=white];\n");
    fprintf(file, "  edge [color=gray];\n");

    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {
            fprintf(file, "  \"%s\" [label=\"%s\"];\n",
                graph->nodes[i].url,
                graph->nodes[i].url);
        }
        
        for (int j = 0; j < graph->nodes[i].numEdges; j++) {
            fprintf(file, "  \"%s\" -> \"%s\" [label=\"%d\"];\n",
                graph->nodes[i].url,
                graph->nodes[graph->nodes[i].edges[j].dest].url,
                graph->nodes[i].edges[j].weight);
        }
    }
    if (path && path->length > 1) {
        for (int i = 0; i < path->length; i++) {
            fprintf(file, "  \"%s\" [fillcolor=lightblue];\n",
                graph->nodes[path->path[i]].url);
        }

        for (int i = 0; i < path->length - 1; i++) {
            int from = path->path[i];
            int to = path->path[i + 1];
            fprintf(file, "  \"%s\" -> \"%s\" [color=blue,penwidth=2.0];\n",
                graph->nodes[from].url,
                graph->nodes[to].url);
        }

        fprintf(file, "  subgraph cluster_legend {\n");
        fprintf(file, "    label=\"Legend\";\n");
        fprintf(file, "    style=dotted;\n");
        fprintf(file, "    \"Path Node\" [fillcolor=lightblue];\n");
        fprintf(file, "    \"Other Node\" [fillcolor=white];\n");
        fprintf(file, "    \"Path Node\" -> \"Other Node\" [style=invis];\n");
        fprintf(file, "  }\n");
    }

    fprintf(file, "}\n");
    fclose(file);
}

void freePath(Path* path) {
    if (path != NULL) {
        free(path->path);
        free(path);
    }
}