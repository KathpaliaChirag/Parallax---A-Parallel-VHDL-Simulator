#ifndef GRAPH_H
#define GRAPH_H
typedef struct {
    int num_nodes;
    int num_edges;
    int **adj;  // dynamically allocated in graph_build
    int * color;
    int num_colors;
} DepGraph;
DepGraph* graph_build(int num_processes);
void graph_add_edge(DepGraph* g, int proc_a, int proc_b);
void graph_color(DepGraph* g);
void graph_print(DepGraph* g);
void graph_free(DepGraph* g);
// okay so just stole an idea lol..... well why not display graph XD lets do that too hahahaha
void graph_write_dot(DepGraph* g, const char* filename);
#endif