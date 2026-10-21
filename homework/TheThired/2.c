#include <stdio.h>
#include <stdlib.h>

#define MAXN 10000
#define MAXM 100

static int coins[MAXN];
static unsigned char can[MAXN + 1][MAXM + 1];

static int compare(const void *a, const void *b)
{
    int x = *(const int *)a;
    int y = *(const int *)b;

    return (x > y) - (x < y);
}

int main(void)
{
    int n, m, i, s;
    int remaining, first = 1;

    if (scanf("%d%d", &n, &m) != 2)
        return 0;

    for (i = 0; i < n; ++i)
    {
        if (scanf("%d", &coins[i]) != 1)
            return 0;
    }

    qsort(coins, (size_t)n, sizeof(coins[0]), compare);

    /* 没有硬币时，只能凑出金额 0 */
    can[n][0] = 1;

    /* 从后向前计算每个后缀能够凑出的金额 */
    for (i = n - 1; i >= 0; --i)
    {
        for (s = 0; s <= m; ++s)
        {
            /* 不选当前硬币 */
            can[i][s] = can[i + 1][s];

            /* 选当前硬币 */
            if (s >= coins[i] && can[i + 1][s - coins[i]])
                can[i][s] = 1;
        }
    }

    if (!can[0][m])
    {
        puts("No Solution");
        return 0;
    }

    /* 优先选取较小且不会导致后续无解的硬币 */
    remaining = m;

    for (i = 0; i < n && remaining > 0; ++i)
    {
        if (coins[i] <= remaining &&
            can[i + 1][remaining - coins[i]])
        {
            if (!first)
                printf(" ");

            printf("%d", coins[i]);
            first = 0;
            remaining -= coins[i];
        }
    }

    printf("\n");
    return 0;
}