#include <stdio.h>
#include <string.h>
#include <limits.h>

#define MAXN 105
#define MAXE 10005

typedef struct
{
    int to, next;
    long long capacity;
} Edge;

static Edge edges[MAXE];
static int head[MAXN], current[MAXN];
static int level[MAXN], queue[MAXN];
static int edgeCount;

/* 正向边与反向边相邻存储，下标通过 ^1 相互转换 */
static void addEdge(int u, int v, long long capacity)
{
    edges[edgeCount].to = v;
    edges[edgeCount].capacity = capacity;
    edges[edgeCount].next = head[u];
    head[u] = edgeCount++;

    edges[edgeCount].to = u;
    edges[edgeCount].capacity = 0;
    edges[edgeCount].next = head[v];
    head[v] = edgeCount++;
}

/* BFS 建立分层图 */
static int bfs(int source, int sink)
{
    int front = 0, rear = 0;

    memset(level, -1, sizeof(level));
    level[source] = 0;
    queue[rear++] = source;

    while (front < rear)
    {
        int u = queue[front++];
        int e;

        for (e = head[u]; e != -1; e = edges[e].next)
        {
            int v = edges[e].to;

            if (edges[e].capacity > 0 && level[v] == -1)
            {
                level[v] = level[u] + 1;
                queue[rear++] = v;
            }
        }
    }

    return level[sink] != -1;
}

/* 只沿分层图向下一层增广 */
static long long dfs(int u, int sink, long long limit)
{
    int *p;

    if (u == sink)
        return limit;

    /* 当前弧优化：避免反复检查已经无法增广的边 */
    for (p = &current[u]; *p != -1; *p = edges[*p].next)
    {
        int e = *p;
        int v = edges[e].to;

        if (edges[e].capacity > 0 && level[v] == level[u] + 1)
        {
            long long available = limit < edges[e].capacity
                                      ? limit
                                      : edges[e].capacity;
            long long pushed = dfs(v, sink, available);

            if (pushed > 0)
            {
                edges[e].capacity -= pushed;
                edges[e ^ 1].capacity += pushed;
                return pushed;
            }
        }
    }

    return 0;
}

static long long maxFlow(int source, int sink)
{
    long long total = 0;

    if (source == sink)
        return 0;

    while (bfs(source, sink))
    {
        long long pushed;

        memcpy(current, head, sizeof(head));

        while ((pushed = dfs(source, sink, LLONG_MAX / 4)) > 0)
            total += pushed;
    }

    return total;
}

int main(void)
{
    int n, m, source, sink, i;

    if (scanf("%d%d%d%d", &n, &m, &source, &sink) != 4)
        return 0;

    memset(head, -1, sizeof(head));

    for (i = 0; i < m; ++i)
    {
        int u, v;
        long long capacity;

        if (scanf("%d%d%lld", &u, &v, &capacity) != 3)
            return 0;

        addEdge(u, v, capacity);
    }

    printf("%lld\n", maxFlow(source, sink));

    return 0;
}