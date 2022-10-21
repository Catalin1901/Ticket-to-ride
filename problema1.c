#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define N 18 // nr_noduri
#define INF 2712380
#define no_edges 36

int graph[N][N];
int dist[20];
int matrice_dijkstra[N][N];

typedef struct bilet
{
    int sursa;
    int dest;
    int punctaj;
} bilet;

typedef struct drum
{
    int sursa;
    int destinatie;
} drum;

typedef struct nodP
{
    int dist;
    int prev;
    int viz;
} nodP;

typedef struct edge
{
    int source;
    int destination;
    int cost;
} edge;

drum previous[20];
int k = 0;

void verif(FILE *f)
{
    if (f == NULL)
    {
        puts("Fisierul nu se poate deschide!");
        exit(1);
    }
}

void write(edge road[])
{
    FILE *f;
    int cost;
    int i, n1, n2;

    f = fopen("date.txt", "rt");
    verif(f);

    for (i = 0; i < 36; i++)
    {
        fscanf(f, "%d", &road[i].source);
        fscanf(f, "%d", &road[i].destination);
        fscanf(f, "%d", &road[i].cost);
    }
    fclose(f);
}

void init(int source)
{
    int i;

    for (i = 1; i <= N; i++)
        if (i == source)
            dist[source] = 0; // initializam distanta ,nod de inceput e zero
        else
            dist[i] = INT_MAX; // si pe restul cu infinit
}

int minDistance(struct nodP *nodP)
{

    int min = INF, min_index, v;

    for (v = 0; v < N; v++)
        if (!nodP[v].viz && nodP[v].dist <= min)
        {
            min = nodP[v].dist;
            min_index = v;
        }

    return min_index;
}

int punctaj(int points)
{
    if (points == 1)
        return 1;
    if (points == 2)
        return 2;
    if (points == 3)
        return 4;
    if (points == 4)
        return 7;
}

// DE VERIFICAT CAZUL 2 (BUCKINGHAM TO BRICK LANE)
void Dijkstra(edge *edges, int edge_nr, int source, int destination, char *mask[], int muchii[], int *nr_muchii)
{
    if (source == destination)
    {
        printf("Lungimea drumului de la un nod la el insusi este 0.\n");
        return;
    }
    struct nodP nodP[N];
    int i, j, v;

    for (i = 0; i < N; i++)
    {
        nodP[i].dist = INF;
        nodP[i].prev = -1; // init
        nodP[i].viz = 0;
    }
    source--;
    destination--;
    nodP[source].dist = 0; // nodP[source].viz = 1;

    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            matrice_dijkstra[i][j] = 0;
    for (i = 0; i < edge_nr; i++)
    {
        matrice_dijkstra[edges[i].source - 1][edges[i].destination - 1] = edges[i].cost;
        matrice_dijkstra[edges[i].destination - 1][edges[i].source - 1] = edges[i].cost; // construieste matricea
    }

    for (i = 0; i < N - 1; i++)
    {

        int u = minDistance(nodP);

        nodP[u].viz = 1;

        for (v = 0; v < N; v++)
            if (!nodP[v].viz && matrice_dijkstra[u][v] && nodP[u].dist != INF && nodP[u].dist + matrice_dijkstra[u][v] < nodP[v].dist)
            {
                nodP[v].dist = nodP[u].dist + matrice_dijkstra[u][v];
                nodP[v].prev = u;
            }
    }

    int k = 0;
    edge get_points;

    printf("Cel mai scurt drum de la '%s' la '%s' este:\n", mask[source + 1], mask[destination + 1]);
    // printf("%d ", destination+1);

    int copie = destination;
    while (destination != source)
    {
        // printf("<- %d ", nodP[destination].prev+1);
        muchii[k++] = destination;
        destination = nodP[destination].prev;
    }
    muchii[k] = destination;
    *nr_muchii = k;

    printf("Trenulete folosite pentru drum: %d\n", nodP[copie].dist);
}

int main()
{

    int muchii[20];
    edge edges[36];

    write(edges);

  

    bilet ticket[4] = {{0, 0, 0}, {1, 10, 5}, {2, 17, 9}, {8, 2, 6}};
    // mastile pentru statii
    char *statii[] = {"First elem", "Big Ben", "Buckingham Palace", "Hyde Park", "Baker Street", "Regent's Park", "Piccadilly Circus", "British Museum", "King's Cross", "Covent Garden", "The Charterhouse", "Trafalgar Square", "Saint Paul", "Waterloo", "Elephant&Castle", "Globe Theatre", "Tower of London", "Brick Lane"};

    int nr_muchii;
    int pctj[4] = {0};
    int autobuze = 30;

    //Lucram pentru fiecare ticket in parte
    for (int i = 0; i < 3; i++)
    {
        //DE MODIFICAT DIJKSTRA UL CA SA AFISEZ AICI "CEL MAI SCURT DRUM DE LA......"
        Dijkstra(edges, no_edges, ticket[i + 1].sursa, ticket[i + 1].dest, statii, muchii, &nr_muchii);

        printf("Parcursul complet al drumului este: ");
        for (int j = nr_muchii; j > 0; j--)
        {
            printf("%s", statii[muchii[j]+1]);
            if(j > 0)
            printf(" -> ");

            for (int k = 0; k < no_edges; k++)
            {
                if (edges[k].source == muchii[j] + 1)

                    if (edges[k].destination == muchii[j - 1] + 1)
                    {
                       pctj[i] += punctaj(edges[k].cost);
                       autobuze -= edges[k].cost;
                       break;
                    }

                //Cauta in caz ca muchiile sunt inversate fata de fisierul "date.txt"
                if (edges[k].destination == muchii[j] + 1)
                    if (edges[k].source == muchii[j - 1] + 1)
                    {
                        pctj[i] += punctaj(edges[k].cost);
                        autobuze -= edges[k].cost;
                        break;
                    }
            }
        }
        printf("%s", statii[ticket[i+1].dest]);
        pctj[i] += ticket[i+1].punctaj;
        printf("\nPunctajul final pentru drumul %d este: %d", i+1, pctj[i]);
        printf("\n\n");
    }
    
    int punctaj_final = pctj[0] + pctj[1] + pctj[2] + (30 - autobuze);
    printf("Autobuze folosite: %d\n", 30 - autobuze);
    printf("Scorul final al jucatorului: %d", punctaj_final);

    
    return 0;
}
