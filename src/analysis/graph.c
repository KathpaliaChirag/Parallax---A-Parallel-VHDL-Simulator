//CHIRAG 04-04-26 18:29 ::
// okay so idea is simple we will allocate graph struct and
//initialize it to default like clr array to -1
// rest to 0 
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"

// CHIRAG 04-04-26 :: allocates a new dependency graph for num_processes nodes
// adj is a 2D array ... malloc rows first then each row individually
// color array starts all -1 meaning uncolored
DepGraph* graph_build(int num_processes)
{
    DepGraph* g = malloc(sizeof(DepGraph));
    g->num_nodes = num_processes;
    g->num_edges = 0;
    g->num_colors = 0;

    // malloc each row separately ... this is how 2D dynamic arrays work in C
    g->adj = malloc(num_processes * sizeof(int*));
    for(int i = 0; i < num_processes; i++)
    {
        g->adj[i] = malloc(num_processes * sizeof(int));
        for(int j = 0; j < num_processes; j++)
            g->adj[i][j] = 0;
    }

    g->color = malloc(num_processes * sizeof(int));
    for(int i = 0; i < num_processes; i++)
        g->color[i] = -1;

    return g;
}

void graph_add_edge(DepGraph* g, int proc_a, int proc_b)
{
    g->adj[proc_a][proc_b]=1;
    g->adj[proc_b][proc_a]=1;
    g->num_edges++;
}

void graph_color(DepGraph* g)
{
    // CHIRAG 04-04-26 :: Welsh-Powell graph coloring algorithm
    // AI assisted...took help here to understanding documented here
    // why do we need this?
    // we have processes and some conflict with each other (share signals)
    // we want to group processes into batches where nobody in a batch conflicts
    // each batch can run in parallel safely
    // step 1 ... count degree of each process
    // degree = how many other processes it conflicts with
    // more conflicts = harder to color = should be colored first
    int degree[g->num_nodes];
    for(int i = 0; i < g->num_nodes; i++)
    {
        degree[i] = 0;
        for(int j = 0; j < g->num_nodes; j++)
            degree[i] += g->adj[i][j];
    }

    // step 2 ... sort processes by degree descending
    // we use a simple index array so we dont move actual data around
    // sorted[0] = index of process with highest degree
    // sorted[1] = index of process with second highest degree ... etc
    int sorted[g->num_nodes];
    for(int i = 0; i < g->num_nodes; i++)
        sorted[i] = i;
    // bubble sort ... small N so fine
    for(int i = 0; i < g->num_nodes; i++)
        for(int j = i+1; j < g->num_nodes; j++)
            if(degree[sorted[j]] > degree[sorted[i]])
            {
                int tmp = sorted[i];
                sorted[i] = sorted[j];
                sorted[j] = tmp;
            }

    // step 3 ... assign colors greedily
    // for each process in sorted order ... find smallest color not used by any neighbor
    // neighbor_has_color[c] = 1 means a neighbor already has color c ... cant use it
    for(int i = 0; i < g->num_nodes; i++)
    {
        int proc = sorted[i];
        int neighbor_has_color[g->num_nodes];
        for(int c = 0; c < g->num_nodes; c++)
            neighbor_has_color[c] = 0;

        // mark colors used by neighbors
        for(int j = 0; j < g->num_nodes; j++)
            if(g->adj[proc][j] == 1 && g->color[j] != -1)
                neighbor_has_color[g->color[j]] = 1;

        // pick smallest color not used by any neighbor
        int c = 0;
        while(neighbor_has_color[c]) c++;
        g->color[proc] = c;

        // track total number of colors used
        if(c + 1 > g->num_colors)
            g->num_colors = c + 1;
    }
}

void graph_print(DepGraph* g)
{
    printf("num nodes: %d\n", g->num_nodes);
    printf("num edges: %d\n", g->num_edges);
    printf("num colors: %d\n", g->num_colors);

    // CHIRAG 04-04-26 :: adjacency matrix ... 1 means conflict between process i and j
    printf("adjacency matrix:\n");
    for(int i = 0; i < g->num_nodes; i++)
    {
        for(int j = 0; j < g->num_nodes; j++)
            printf("%d ", g->adj[i][j]);
        printf("\n");
    }

    // color per process
    printf("colors:\n");
    for(int i = 0; i < g->num_nodes; i++)
        printf("proc %d ... color %d\n", i, g->color[i]);

    // CHIRAG 04-04-26 :: color classes ... processes with same color run in parallel
    printf("parallel batches:\n");
    for(int c = 0; c < g->num_colors; c++)
    {
        printf("batch %d: ", c);
        for(int i = 0; i < g->num_nodes; i++)
            if(g->color[i] == c)
                printf("proc%d ", i);
        printf("\n");
    }
}

void graph_free(DepGraph* g)
{
    // CHIRAG 04-04-26 :: free in reverse order of allocation
    // free each row of adj first ... then the row pointer array ... then color ... then struct itself
    for(int i = 0; i < g->num_nodes; i++)
        free(g->adj[i]);
    free(g->adj);
    free(g->color);
    free(g);
}