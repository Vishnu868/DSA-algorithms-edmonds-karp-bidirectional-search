#include "edgraph.h"

int main() {
    Graph* graph = createGraph(MAX_VERTICES);
    char filename[256];
    
    printf("Enter the filename containing URLs and links: ");
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Error reading filename\n");
        freeGraph(graph);
        return 1;
    }
    filename[strcspn(filename, "\n")] = '\0';

    FILE* test = fopen(filename, "r");
    if (!test) {
        printf("Error: File '%s' does not exist or cannot be opened\n", filename);
        freeGraph(graph);
        return 1;
    }
    fclose(test);
    
    processUrlFile(graph, filename);
    
    //print vertices
    int hasVertices = 0;
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url != NULL) {
            hasVertices = 1;
            break;
        }
    }
    
    if (hasVertices) {
        //graph representations
        printf("\n=== Original Graph Structure ===\n");
        printWeightedEdgeList(graph);
        printAdjacencyList(graph);
        printAdjacencyMatrix(graph);
        
        // Run Edmonds-Karp algorithm
        printf("\n=== Maximum Flow Analysis ===\n");
        char sourceUrl[MAX_URL_LENGTH], sinkUrl[MAX_URL_LENGTH];
        
        printf("Enter source URL: ");
        if (fgets(sourceUrl, sizeof(sourceUrl), stdin) == NULL) {
            printf("Error reading source URL\n");
            freeGraph(graph);
            return 1;
        }
        sourceUrl[strcspn(sourceUrl, "\n")] = '\0';
        
        printf("Enter sink URL: ");
        if (fgets(sinkUrl, sizeof(sinkUrl), stdin) == NULL) {
            printf("Error reading sink URL\n");
            freeGraph(graph);
            return 1;
        }
        sinkUrl[strcspn(sinkUrl, "\n")] = '\0';
        
        int maxFlow = edmondsKarp(graph, sourceUrl, sinkUrl);
        if (maxFlow >= 0) {
            printf("\nMaximum flow from %s to %s: %d\n", sourceUrl, sinkUrl, maxFlow);
            
            // Print residual graph after max flow calculation
            printf("\n=== Residual Graph After Maximum Flow ===\n");
            printWeightedEdgeList(graph);
            printAdjacencyList(graph);
            printAdjacencyMatrix(graph);
        } else {
            printf("Error: Could not compute maximum flow. Check if URLs exist in the graph.\n");
        }
        
        printf("\nBroken Links:\n");
        int brokenCount = 0;
        for (int i = 0; i < graph->numVertices; i++) {
            if (graph->nodes[i].numEdges == 0 && graph->nodes[i].url != NULL) {
                printf("%s\n", graph->nodes[i].url);
                brokenCount++;
            }
        }
        if (brokenCount == 0) {
            printf("No broken links found\n");
        }
        
        writeGraphToDot(graph, "graph.dot");
        printf("Graph has been written to graph.dot\n");
    } else {
        printf("No vertices were loaded from the file\n");
    }
    
    freeGraph(graph);
    return 0;
}