#include <stdio.h>
#include <string.h>

#define MAXS 10000
#define MAXC 101
#define MAXE 20005
#define INF 0x3f3f3f3f

typedef struct
{
    int to, company, next;
} Edge;

static Edge edges[MAXE];
static int head[MAXS], edgeCount;

static int dist[MAXS], queue[MAXS];

/* changes[u][c]：沿最短路径乘公司 c 到达 u 的最少换乘数 */
static int changes[MAXS][MAXC];
static int prevStation[MAXS][MAXC];
static int prevCompany[MAXS][MAXC];

static int path[MAXS + 1];
static int pathCompany[MAXS + 1];

static void addEdge(int u, int v, int company)
{
    edges[edgeCount].to = v;
    edges[edgeCount].company = company;
    edges[edgeCount].next = head[u];
    head[u] = edgeCount++;
}

static void solve(int start, int target, int companyCount)
{
    int front = 0, rear = 0;
    int i, e, last, best, u, line, segmentStart;

    /* 第一步：BFS 求最短距离 */
    memset(dist, -1, sizeof(dist));
    dist[start] = 0;
    queue[rear++] = start;

    while (front < rear)
    {
        u = queue[front++];

        for (e = head[u]; e != -1; e = edges[e].next)
        {
            int v = edges[e].to;

            if (dist[v] == -1)
            {
                dist[v] = dist[u] + 1;
                queue[rear++] = v;
            }
        }
    }

    if (dist[target] == -1)
    {
        puts("Sorry, no line is available.");
        return;
    }

    if (start == target)
    {
        puts("0");
        return;
    }

    /* 第二步：在最短路径中求最少换乘次数 */
    memset(changes, 0x3f, sizeof(changes));
    changes[start][0] = 0; /* 0 表示尚未乘坐任何线路 */

    /* BFS 队列中的站点按距离从小到大排列 */
    for (i = 0; i < rear; ++i)
    {
        u = queue[i];

        if (dist[u] >= dist[target])
            continue;

        for (e = head[u]; e != -1; e = edges[e].next)
        {
            int v = edges[e].to;
            int c = edges[e].company;

            /* 只沿最短路径前进 */
            if (dist[v] != dist[u] + 1)
                continue;

            for (last = 0; last <= companyCount; ++last)
            {
                int candidate;

                if (changes[u][last] == INF)
                    continue;

                candidate = changes[u][last] + (last != 0 && last != c);

                if (candidate < changes[v][c])
                {
                    changes[v][c] = candidate;
                    prevStation[v][c] = u;
                    prevCompany[v][c] = last;
                }
            }
        }
    }

    /* 找到到达终点时的最优线路状态 */
    best = 1;
    for (line = 2; line <= companyCount; ++line)
    {
        if (changes[target][line] < changes[target][best])
            best = line;
    }

    /* 反向还原完整路径 */
    u = target;
    line = best;

    for (i = dist[target]; i > 0; --i)
    {
        int p = prevStation[u][line];
        int c = prevCompany[u][line];

        path[i] = u;
        pathCompany[i] = line;

        u = p;
        line = c;
    }
    path[0] = start;

    printf("%d\n", dist[target]);

    /* 同一公司的连续路段合并输出 */
    segmentStart = 0;

    for (i = 1; i <= dist[target]; ++i)
    {
        if (i == dist[target] ||
            pathCompany[i] != pathCompany[i + 1])
        {
            printf("Go by the line of company #%d "
                   "from %04d to %04d.\n",
                   pathCompany[i],
                   path[segmentStart], path[i]);

            segmentStart = i;
        }
    }
}

int main(void)
{
    int n, k, i, j;

    if (scanf("%d", &n) != 1)
        return 0;

    memset(head, -1, sizeof(head));

    for (i = 1; i <= n; ++i)
    {
        int m, previous;

        if (scanf("%d%d", &m, &previous) != 2)
            return 0;

        for (j = 1; j < m; ++j)
        {
            int current;

            if (scanf("%d", &current) != 1)
                return 0;

            /* 线路可以双向运行 */
            addEdge(previous, current, i);
            addEdge(current, previous, i);

            previous = current;
        }
    }

    if (scanf("%d", &k) != 1)
        return 0;

    while (k--)
    {
        int start, target;

        if (scanf("%d%d", &start, &target) != 2)
            return 0;

        solve(start, target, n);
    }

    return 0;
}