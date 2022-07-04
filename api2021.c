#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct
{
    unsigned node;
    int priority;
}queue_el;

typedef struct
{
    unsigned graph_num;
    unsigned score;
}ranking_el;

void create_matrix_graph (unsigned ** graph, int dim);
unsigned my_strtok_and_atoi (char * str, int * initial_pos, int valid);
void modify_ranking_max_heap (ranking_el * ranking, unsigned  graph_score, unsigned graph_id, unsigned k);
void build_max_heap (ranking_el * ranking, int dim);
void Max_heapify (ranking_el * ranking, unsigned pos, int heap_size);
void DijkstraQueue (unsigned ** graph, int * vector_distance, int dim);
void Enqueue_priority (queue_el * Q, unsigned el, int priority, int heap_size);
unsigned Delete_minimum (queue_el * Q, int heap_size);
void Decrement_priority(queue_el * Q, unsigned el, int priority);
void Min_heapify (queue_el * Q, unsigned pos, int heap_size);


int main ()
{
    unsigned ** graph;
    unsigned sum_paths;
    ranking_el * ranking;
    int * minimum_paths, i, num_nodes, k, j = 0, el_to_print;
    char command[15];
    if (scanf ("%d %d", &num_nodes, &k) == EOF)
        exit(1);
    minimum_paths = (int *) malloc (sizeof (int ) * num_nodes);
    ranking = (ranking_el *) malloc (sizeof (ranking_el) * k);
    graph = (unsigned **) malloc (sizeof (unsigned *) * num_nodes);
    for (i = 0; i < num_nodes; i++)
        graph[i] = (unsigned *) malloc (sizeof (unsigned) * num_nodes);
    while (scanf ("%s", command) != EOF)
    {
        if (strcmp(command, "AggiungiGrafo") == 0)
        {
            sum_paths = 0;
            create_matrix_graph(graph, num_nodes);
            DijkstraQueue (graph, minimum_paths, num_nodes);
            for (i = 0; i < num_nodes; i++)
            {
                if (minimum_paths[i] == INT_MAX)
                    minimum_paths[i] = 0;
                sum_paths += minimum_paths[i];
            }
            modify_ranking_max_heap (ranking, sum_paths, j, k);
            j++;
        }
        else if (strcmp(command, "TopK") == 0)
        {
            if (j < k)
                el_to_print = j;
            else el_to_print = k;
            if (j != 0)
                printf ("%d", ranking[0].graph_num);
            for (i = 1; i < el_to_print; i++)
                printf (" %d", ranking[i].graph_num);
            printf("\n");
        }
    }
    free(minimum_paths);
    free(ranking);
    for (i = 0; i < num_nodes; i++)
        free(graph[i]);
    free(graph);
    return 0;
}

void create_matrix_graph (unsigned **graph, int dim)
{
    char * str_temp = (char *) malloc (sizeof (char) * 11 * dim);
    char c;
    int i, j, valid, initial_pos;
    for (i = 0; i < dim; i++)
    {
        if (i == 0) //mi rimane da leggere un \n da AggiungiGrafo
            c = getchar();
        if (c != '\n')
            exit(1);
        if (fgets (str_temp, 11 * dim, stdin) == NULL)
            exit(1);
        initial_pos = 0;
        for (j = 0; j < dim; j++)
        {
            if (j == 0 || i == j) //non servono gli autoanelli e la prima colonna
                valid = 0;
            else valid = 1;
            graph[i][j] = my_strtok_and_atoi(str_temp + initial_pos, &initial_pos, valid);
        }
    }
    free (str_temp);
}

unsigned my_strtok_and_atoi (char * str, int * initial_pos, int valid)
{
    int len = 0, i;
    unsigned num = 0, pot = 1;
    for (i = 0; str[i] != ',' && str[i] != '\n' && str[i] != '\0'; i++)
        len++;
    * initial_pos += len + 1;
    if (!valid)
        return 0;
    for (i = len - 1; i >= 0; i--)
    {
        num += (str[i] - '0') * pot;
        pot *= 10;
    }
    return num;
}

void modify_ranking_max_heap (ranking_el * ranking, unsigned  graph_score, unsigned graph_id, unsigned k)
{
    if (graph_id < k)
    {
        ranking[graph_id].score = graph_score;
        ranking[graph_id].graph_num = graph_id;
    }
    else
    {
        if (graph_id == k)
            build_max_heap (ranking, (int) k);
        if (graph_score < ranking[0].score)
        {
            ranking[0].graph_num = graph_id;
            ranking[0].score = graph_score;
            Max_heapify (ranking, 0, (int) k);
        }
    }
}

void build_max_heap (ranking_el * ranking, int dim)
{
    int i;
    for (i = (dim - 1) / 2; i > 0; i--)
        Max_heapify (ranking, i, dim);
}

void Max_heapify (ranking_el * ranking, unsigned pos, int heap_size)
{
    unsigned l = 2 * pos + 1, r = 2 * pos + 2, position_max;
    ranking_el tmp;
    if (l < heap_size && ranking[l].score > ranking[pos].score)
        position_max = l;
    else position_max = pos;
    if (r < heap_size && ranking[r].score > ranking[position_max].score)
        position_max = r;
    if (position_max != pos)
    {
        tmp = ranking[pos];
        ranking[pos] = ranking[position_max];
        ranking[position_max] = tmp;
        Max_heapify (ranking, position_max, heap_size);
    }
}

void DijkstraQueue (unsigned ** graph, int * vector_distance, int dim)
{
    queue_el * Q = (queue_el *) malloc (sizeof (queue_el) * dim);
    int el_in_queue = 0, ndis;
    int i, stop = 0;
    unsigned u;
    for (i = 0; i < dim; i++)
    {
        if (i != 0)
            vector_distance[i] = INT_MAX;
        else vector_distance[i] = 0;
        Enqueue_priority (Q, i, vector_distance[i], el_in_queue);
        el_in_queue++;
    }
    while (el_in_queue != 0 && !stop)
    {
        u = Delete_minimum (Q, el_in_queue);
        el_in_queue--;
        if (vector_distance[u] != INT_MAX)
        {
            for (i = 0; i < dim; i++)
            {
                ndis = vector_distance[u] + (int) graph[u][i];
                if (graph[u][i] != 0 && ndis < vector_distance[i])
                {
                    vector_distance[i] = ndis;
                    Decrement_priority (Q, i, ndis);
                }
            }
        }
        else stop = 1;
    }
    free (Q);
}

void Enqueue_priority (queue_el * Q, unsigned el, int priority, int heap_size)
{
    unsigned i;
    queue_el tmp;
    heap_size++;
    Q[heap_size - 1].node = el;
    Q[heap_size - 1].priority = priority;
    i = heap_size - 1;
    while (i > 0 && Q[(i - 1) / 2].priority > Q[i].priority)
    {
        tmp = Q[i];
        Q[i] = Q[(i - 1) / 2];
        Q[(i - 1) / 2] = tmp;
        i = (i - 1) / 2;
    }
}

unsigned Delete_minimum (queue_el * Q, int heap_size)
{
    unsigned min = Q[0].node;
    if (heap_size > 1)
    {
        Q[0] = Q[heap_size - 1];
        heap_size--;
        Min_heapify (Q, 0, heap_size);
    }
    return min;
}

void Min_heapify (queue_el * Q, unsigned pos, int heap_size)
{
    unsigned l = 2 * pos + 1, r = 2 * pos + 2, position_min;
    queue_el tmp;
    if (l < heap_size && Q[l].priority < Q[pos].priority)
        position_min = l;
    else position_min = pos;
    if (r < heap_size && Q[r].priority < Q[position_min].priority)
        position_min = r;
    if (position_min != pos)
    {
        tmp = Q[pos];
        Q[pos] = Q[position_min];
        Q[position_min] = tmp;
        Min_heapify (Q, position_min, heap_size);
    }
}

void Decrement_priority (queue_el * Q, unsigned el, int priority)
{
    int i = 0;
    queue_el tmp;
    while (Q[i].node != el)
        i++;
    Q[i].priority = priority;
    while (i > 0 && Q[(i - 1) / 2].priority > Q[i].priority)
    {
        tmp = Q[i];
        Q[i] = Q[(i - 1) / 2];
        Q[(i - 1) / 2] = tmp;
        i = (i - 1) / 2;
    }
}