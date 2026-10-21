#include <stdio.h>
#include <string.h>

#define MAXN 1005
#define MAXE 10005
#define INF 0x3f3f3f3f

typedef struct
{
    int to, cost, next;
} Edge;

static Edge edges[MAXE];
static int head[MAXN], edgeCount;
static int heat[MAXN];
static int dist[MAXN], peak[MAXN], visited[MAXN];

static void addEdge(int u, int v, int cost)
{
    edges[edgeCount].to = v;
    edges[edgeCount].cost = cost;
    edges[edgeCount].next = head[u];
    head[u] = edgeCount++;
}

int main(void)
{
    int n, m, start, target;
    int i, j;

    if (scanf("%d%d%d%d", &n, &m, &start, &target) != 4)
        return 0;

    memset(head, -1, sizeof(head));

    for (i = 1; i <= n; ++i)
    {
        if (scanf("%d", &heat[i]) != 1)
            return 0;

        dist[i] = INF;
        peak[i] = INF;
    }

    for (i = 0; i < m; ++i)
    {
        int u, v, cost;

        if (scanf("%d%d%d", &u, &v, &cost) != 3)
            return 0;

        addEdge(u, v, cost);
        addEdge(v, u, cost);
    }

    dist[start] = 0;
    peak[start] = 0;

    for (i = 0; i < n; ++i)
    {
        int u = -1;
        int e;

        /* 选择花销最小的未确定城镇，花销相同时比较热度 */
        for (j = 1; j <= n; ++j)
        {
            if (!visited[j] &&
                (u == -1 || dist[j] < dist[u] ||
                 (dist[j] == dist[u] && peak[j] < peak[u])))
            {
                u = j;
            }
        }

        if (u == -1 || dist[u] == INF)
            break;

        visited[u] = 1;

        if (u == target)
            break;

        for (e = head[u]; e != -1; e = edges[e].next)
        {
            int v = edges[e].to;
            int newDist, newPeak;

            if (visited[v])
                continue;

            newDist = dist[u] + edges[e].cost;
            newPeak = peak[u];

            /* 只统计中间城镇的热度 */
            if (v != start && v != target && heat[v] > newPeak)
                newPeak = heat[v];

            if (newDist < dist[v] ||
                (newDist == dist[v] && newPeak < peak[v]))
            {
                dist[v] = newDist;
                peak[v] = newPeak;
            }
        }
    }

    if (dist[target] == INF)
        puts("Impossible");
    else
        printf("%d %d\n", dist[target], peak[target]);

    return 0;
}