#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bdgraph.h"



int main(int argc, char* argv[]) {
    Graph* graph = createGraph(MAX_VERTICES);
    char source_url[MAX_URL_LENGTH];
    char target_url[MAX_URL_LENGTH];
    char filename[256];
    
    if (argc > 1) {
        strncpy(filename, argv[1], sizeof(filename) - 1);
        filename[sizeof(filename) - 1] = '\0';
    } else {
        printf("Enter the filename containing URLs and links: ");
        if (scanf("%255s", filename) != 1) {
            printf("Error reading filename\n");
            freeGraph(graph);
            return 1;
        }
    }
    
    processUrlFile(graph, filename);
    printf("Finished reading file. Processed %d vertices\n", graph->numVertices);
    
    printf("\n=== Adjacency List ===\n");
    printAdjacencyList(graph);
    
    printf("\n=== Adjacency Matrix ===\n");
    printAdjacencyMatrix(graph);
    
    printf("\n=== Weighted Edge List ===\n");
    printWeightedEdgeList(graph);
    
    while (getchar() != '\n');
    
    printf("\n=== Available URLs in the Graph ===\n");
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url) {
            printf("%d. %s\n", i + 1, graph->nodes[i].url);
        }
    }
    
    while (1) {
        printf("\n=== Bidirectional Search for Shortest Path ===\n");
        printf("Available URLs:\n");
        for (int i = 0; i < graph->numVertices; i++) {
            if (graph->nodes[i].url) {
                printf("%d. %s\n", i + 1, graph->nodes[i].url);
            }
        }
        
        printf("\nEnter source URL (or 'quit' to exit): ");
        if (fgets(source_url, MAX_URL_LENGTH, stdin) == NULL) break;
        source_url[strcspn(source_url, "\n")] = 0;
        
        if (strcmp(source_url, "quit") == 0) break;
        
        // Validate source URL
        if (findVertexByUrl(graph, source_url) == -1) {
            printf("Error: Source URL '%s' not found in graph\n", source_url);
            continue;
        }
        
        printf("Enter target URL: ");
        if (fgets(target_url, MAX_URL_LENGTH, stdin) == NULL) break;
        target_url[strcspn(target_url, "\n")] = 0;
        
        // Validate target URL
        if (findVertexByUrl(graph, target_url) == -1) {
            printf("Error: Target URL '%s' not found in graph\n", target_url);
            continue;
        }
        
        printf("\nSearching for shortest path from:\n");
        printf("Source: %s\n", source_url);
        printf("Target: %s\n\n", target_url);
        
        //bidirectional search
        Path* shortest_path = bidirectionalSearch(graph, source_url, target_url);
        
        if (shortest_path != NULL) {
            printf("\n=== Shortest Path Found ===\n");
            printPathDetails(graph, shortest_path);
            
            char* source_name = strrchr(source_url, '/');
            source_name = source_name ? source_name + 1 : source_url;
            char* target_name = strrchr(target_url, '/');
            target_name = target_name ? target_name + 1 : target_url;
            
            char viz_filename[256];
            char source_part[64], target_part[64];
            
            strncpy(source_part, source_name, 63);
            source_part[63] = '\0';
            strncpy(target_part, target_name, 63);
            target_part[63] = '\0';
            
            snprintf(viz_filename, sizeof(viz_filename), "path_%.63s_to_%.63s.dot", 
                     source_part, target_part);
            
            visualizeBidirectionalPath(graph, shortest_path, viz_filename);
            printf("\nPath visualization has been written to %s\n", viz_filename);
            
            // Calculate and display path metrics
            int total_weight = 0;
            for (int i = 0; i < shortest_path->length - 1; i++) {
                int from = shortest_path->path[i];
                int to = shortest_path->path[i + 1];
                total_weight += graph->adjMatrix[from][to];
            }
            
            printf("\nPath Statistics:\n");
            printf("Number of hops: %d\n", shortest_path->length - 1);
            printf("Total path weight: %d\n", total_weight);
            if (shortest_path->length > 1) {
                printf("Average weight per hop: %.2f\n", 
                       (float)total_weight / (shortest_path->length - 1));
            }
            
            freePath(shortest_path);
        } else {
            printf("\nNo path found between %s and %s\n", source_url, target_url);
        }
        
        printf("\nWould you like to find another path? (yes/no): ");
        char response[10];
        if (fgets(response, sizeof(response), stdin) != NULL) {
            response[strcspn(response, "\n")] = 0;
            if (strcmp(response, "no") == 0 || strcmp(response, "n") == 0) {
                break;
            }
        }
    }

    printf("\n=== Dead End Analysis ===\n");
    printf("URLs with no outgoing links:\n");
    int dead_ends = 0;
    for (int i = 0; i < graph->numVertices; i++) {
        if (graph->nodes[i].url && graph->nodes[i].numEdges == 0) {
            printf("- %s\n", graph->nodes[i].url);
            dead_ends++;
        }
    }
    if (dead_ends == 0) {
        printf("No dead ends found in the graph.\n");
    } else {
        printf("Total dead ends: %d\n", dead_ends);
    }
    
    writeGraphToDot(graph, "final_graph.dot");
    printf("\nComplete graph visualization has been written to final_graph.dot\n");
    
    freeGraph(graph);
    return 0;
}
