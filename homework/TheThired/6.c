#include <stdio.h>
#include <limits.h>

#define MAXN 100
#define INF INT_MAX

static int graph[MAXN][MAXN];
static int dist[MAXN];

static int bellmanFord(int n)
{
    int i, u, v;

    for (i = 0; i < n; ++i)
        dist[i] = INF;

    dist[0] = 0;

    /* 执行 n-1 轮松弛，顺序为起点升序、终点升序 */
    for (i = 1; i < n; ++i)
    {
        for (u = 0; u < n; ++u)
        {
            if (dist[u] == INF)
                continue;

            for (v = 0; v < n; ++v)
            {
                if (graph[u][v] != INF &&
                    dist[u] + graph[u][v] < dist[v])
                {
                    dist[v] = dist[u] + graph[u][v];
                }
            }
        }
    }

    /* 检查从源点可达的负环，不再修改距离数组 */
    for (u = 0; u < n; ++u)
    {
        if (dist[u] == INF)
            continue;

        for (v = 0; v < n; ++v)
        {
            if (graph[u][v] != INF &&
                dist[u] + graph[u][v] < dist[v])
            {
                return 0;
            }
        }
    }

    return 1;
}

int main(void)
{
    int n, m, i, j, result;

    if (scanf("%d%d", &n, &m) != 2)
        return 0;

    for (i = 0; i < n; ++i)
        for (j = 0; j < n; ++j)
            graph[i][j] = INF;

    for (i = 0; i < m; ++i)
    {
        int u, v, weight;

        if (scanf("%d%d%d", &u, &v, &weight) != 3)
            return 0;

        /* 有向边；重边保留较小权值 */
        if (weight < graph[u][v])
            graph[u][v] = weight;
    }

    result = bellmanFord(n);

    printf("BF returns %d\n", result);

    for (i = 0; i < n; ++i)
        printf("dist[%d] = %d\n", i, dist[i]);

    return 0;
}