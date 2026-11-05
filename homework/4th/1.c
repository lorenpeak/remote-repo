#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAXN 20
#define CACHE_SIZE (1u << 18)

__extension__ typedef unsigned __int128 U128;

typedef struct
{
    uint32_t adj[MAXN];
    uint32_t mask;
} Graph;

typedef struct
{
    Graph graph;
    U128 answer;
    unsigned char used;
} Entry;

static Entry cache[CACHE_SIZE];
static int n, colors;

static void removeVertex(Graph *g, int v)
{
    uint32_t bit = 1u << v;
    uint32_t rest = g->adj[v];

    while (rest)
    {
        int u = __builtin_ctz(rest);
        rest &= rest - 1;
        g->adj[u] &= ~bit;
    }

    g->adj[v] = 0;
    g->mask &= ~bit;
}

static uint32_t hashGraph(const Graph *g)
{
    uint32_t h = g->mask * 2654435761u;
    int i;

    for (i = 0; i < n; ++i)
        h = (h ^ g->adj[i]) * 16777619u;

    return h & (CACHE_SIZE - 1);
}

static U128 count(Graph g)
{
    U128 factor = 1, answer;
    uint32_t todo, component, index;
    int v, a = -1, b = -1;

    /*
     * 若某顶点的邻居两两相邻，则这些邻居必须使用不同颜色。
     * 该顶点有 colors-degree 种选择，可直接删除。
     */
    for (;;)
    {
        int found = 0;
        todo = g.mask;

        while (todo)
        {
            uint32_t neighbors, rest;
            int clique = 1, degree;

            v = __builtin_ctz(todo);
            todo &= todo - 1;
            neighbors = g.adj[v];
            rest = neighbors;

            while (rest)
            {
                int u = __builtin_ctz(rest);
                rest &= rest - 1;

                if (neighbors & ~(g.adj[u] | (1u << u)))
                {
                    clique = 0;
                    break;
                }
            }

            if (!clique)
                continue;

            degree = __builtin_popcount(neighbors);
            if (degree >= colors)
                return 0;

            factor *= colors - degree;
            removeVertex(&g, v);
            found = 1;
            break;
        }

        if (!found)
            break;
    }

    if (!g.mask)
        return factor;

    /* 找出一个连通分量 */
    component = 0;
    todo = g.mask & (0u - g.mask);

    while (todo)
    {
        uint32_t next = 0, scan = todo;
        component |= todo;

        while (scan)
        {
            v = __builtin_ctz(scan);
            scan &= scan - 1;
            next |= g.adj[v];
        }

        todo = next & ~component;
    }

    /* 不同连通分量的方案数相乘 */
    if (component != g.mask)
    {
        Graph left = {{0}, 0};
        Graph right = {{0}, 0};

        left.mask = component;
        right.mask = g.mask ^ component;

        for (v = 0; v < n; ++v)
        {
            if (component & (1u << v))
                left.adj[v] = g.adj[v];
            else
                right.adj[v] = g.adj[v];
        }

        return factor * count(left) * count(right);
    }

    /* 查询缓存，比较完整图结构以处理哈希冲突 */
    index = hashGraph(&g);

    if (cache[index].used &&
        memcmp(&cache[index].graph, &g, sizeof(g)) == 0)
    {
        return factor * cache[index].answer;
    }

    /* 从最小度顶点的邻居中找两个不相邻的顶点 */
    {
        int best = -1, degree = MAXN + 1;
        todo = g.mask;

        while (todo)
        {
            int d;

            v = __builtin_ctz(todo);
            todo &= todo - 1;
            d = __builtin_popcount(g.adj[v]);

            if (d < degree)
            {
                degree = d;
                best = v;
            }
        }

        todo = g.adj[best];

        while (todo && a == -1)
        {
            uint32_t other;

            v = __builtin_ctz(todo);
            todo &= todo - 1;

            other = g.adj[best] & ~(g.adj[v] | (1u << v));

            if (other)
            {
                a = v;
                b = __builtin_ctz(other);
            }
        }
    }

    {
        Graph different = g, same = g;
        uint32_t neighbors, scan;

        /* 情况一：a、b 不同色，添加边 */
        different.adj[a] |= 1u << b;
        different.adj[b] |= 1u << a;

        /* 情况二：a、b 同色，将 b 合并到 a */
        neighbors = (g.adj[a] | g.adj[b]) & ~((1u << a) | (1u << b));

        same.mask &= ~(1u << b);
        scan = same.mask;

        while (scan)
        {
            v = __builtin_ctz(scan);
            scan &= scan - 1;

            same.adj[v] &= ~(1u << b);

            if (neighbors & (1u << v))
                same.adj[v] |= 1u << a;
        }

        same.adj[a] = neighbors;
        same.adj[b] = 0;

        answer = count(different) + count(same);
    }

    cache[index].used = 1;
    cache[index].graph = g;
    cache[index].answer = answer;

    return factor * answer;
}

static void printNumber(U128 x)
{
    char digits[50];
    int length = 0;

    do
    {
        digits[length++] = (char)('0' + x % 10);
        x /= 10;
    } while (x);

    while (length)
        putchar(digits[--length]);

    putchar('\n');
}

int main(void)
{
    Graph g = {{0}, 0};
    int m, i;

    if (scanf("%d%d%d", &n, &m, &colors) != 3)
        return 0;

    g.mask = (1u << n) - 1;

    for (i = 0; i < m; ++i)
    {
        int u, v;

        if (scanf("%d%d", &u, &v) != 2)
            return 0;

        --u;
        --v;

        g.adj[u] |= 1u << v;
        g.adj[v] |= 1u << u;
    }

    printNumber(count(g));

    return 0;
}