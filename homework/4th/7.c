#include <stdio.h>
#include <string.h>

#define MAXN 100

static unsigned char graph[MAXN][MAXN];
static unsigned char visited[MAXN];
static int matchRight[MAXN];
static int leftCount, rightCount;

/* 尝试为左侧顶点 u 寻找增广路 */
static int augment(int u)
{
    int v;

    for (v = 0; v < rightCount; ++v)
    {
        if (graph[u][v] && !visited[v])
        {
            visited[v] = 1;

            /* v 未匹配，或能为原来的匹配对象重新安排匹配 */
            if (matchRight[v] == -1 || augment(matchRight[v]))
            {
                matchRight[v] = u;
                return 1;
            }
        }
    }

    return 0;
}

static int hungarian(void)
{
    int u, v, answer = 0;

    for (v = 0; v < rightCount; ++v)
        matchRight[v] = -1;

    for (u = 0; u < leftCount; ++u)
    {
        memset(visited, 0, sizeof(visited));

        if (augment(u))
            ++answer;
    }

    return answer;
}

int main(void)
{
    int m, i;

    if (scanf("%d%d%d", &leftCount, &rightCount, &m) != 3)
        return 0;

    for (i = 0; i < m; ++i)
    {
        int u, v;

        if (scanf("%d%d", &u, &v) != 2)
            return 0;

        graph[u][v] = 1;
    }

    printf("最大匹配值 = %d\n", hungarian());

    return 0;
}