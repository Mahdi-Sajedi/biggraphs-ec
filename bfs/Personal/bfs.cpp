#include "bfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cstddef>
#include <omp.h>

#include "../common/CycleTimer.h"
#include "../common/graph.h"

#define ROOT_NODE_ID 0
#define NOT_VISITED_MARKER -1

void vertex_set_clear(vertex_set* list) {
    list->count = 0;
}

void vertex_set_init(vertex_set* list, int count) {
    list->max_vertices = count;
    list->vertices = (int*)malloc(sizeof(int) * list->max_vertices);
    vertex_set_clear(list);
}

// Take one step of "top-down" BFS. For each vertex on the frontier,
// follow ll outgoing edges, and add all neighboring vertices to the 
// new_frontier
void top_down_step(
    Graph g,
    vertex_set* frontier,
    vertex_set* new_frontier,
    int* distances)
{
    for (int i=0; i<frontier->count; i++) {

        int node = frontier->vertices[i];

        int start_edge = g->outgoing_starts[node];
        int end_edge = (node == g->num_nodes -1) ? g->num_edges : g->outgoing_starts[node+1];

        // attempt to add all neighbors to the new frontier   
        for (int neighbor=start_edge; neighbor<end_edge; neighbor++) {
            int outgoing = g->outgoing_edges[neighbor];

            if (distances[outgoing] == NOT_VISITED_MARKER) {
                distances[outgoing] = distances[node] + 1;
                new_frontier->vertices[new_frontier->count++] = outgoing;
            }
        }
    }
}

// Implements top-down BFS.
//
// Result of execution: for each node in the graph, the distance to the root is stored in sol.distances
void bfs_top_down(Graph graph, solution* sol) {

    vertex_set list1;
    vertex_set list2;
    vertex_set_init(&list1, graph->num_nodes);
    vertex_set_init(&list2, graph->num_nodes);

    vertex_set* frontier = &list1;
    vertex_set* new_frontier = &list2;

    // initialize all nodes to NOT_VISITED
    for (int i=0; i<graph->num_nodes; i++)
        sol->distances[i] = NOT_VISITED_MARKER;

    // setup frontier with the root node
    frontier->vertices[frontier->count++] = ROOT_NODE_ID;
    sol->distances[ROOT_NODE_ID] = 0;

    while (frontier->count != 0) {
        
#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        vertex_set_clear(new_frontier);
        top_down_step(graph, frontier, new_frontier, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("frontier=%-10d %.4f sec\n", frontier->count, end_time - start_time);
#endif

        // swap pointers
        vertex_set* tmp = frontier;
        frontier = new_frontier;
        new_frontier = tmp;
    }
}

bool bottom_up_step(Graph g, int[] status, int* distances) {
    bool final_step = true;
    for(int i=0; i<g->num_nodes; i++){
        if (distances[i] != NOT_VISITED_MARKER)
            continue;
        int start_edge = g->incoming_starts[i];
        int end_edge = (i == g->num_nodes - 1 ? g->num_edges: g->incoming_starts[i+1]);

        // attempt to find an incoming edge to i whose other endpoint is discovered
        for(int neighbor=start_edge; neighbor<end_edge; neighbor++){
            int incoming = g->incoming_edges[neighbor];

            if (distances[neighbor] != NOT_VISITED_MARKER){
                status[i] = distances[neighbor] + 1;
                final_step = false;
                break;
            }
        }
    }
    return final_step;
}

void bfs_bottom_up(Graph graph, solution* sol)
{
    // refer to handout for details
    // you can implement bottom_up_step() if see fit 

    // initialize all nodes to NOT_VISITED;
    for (int i=0; i<graph->num_nodes; i++){
        sol->distances[i] = NOT_VISITED_MARKER;
    }
    
    //
    int status[graph->num_nodes];
    sol->distances[ROOT_NODE_ID] = 0;

    int is_final_step = false;
    while (!is_final_step){

#ifdef VERBOSE
        double start_time = CycleTimer::currentSeconds();
#endif

        // vertex_set_clear(new_frontier);
        is_final_step = bottom_up_step(graph, status, sol->distances);

#ifdef VERBOSE
        double end_time = CycleTimer::currentSeconds();
        printf("are we done? %-10s %.4f sec\n", is_final_step? "true": "false", end_time - start_time);
#endif
    }
}
