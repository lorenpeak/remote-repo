#include <stdio.h>

#define MAXN 20

static int n, answerLength;
static long long a[MAXN], suffix[MAXN + 1], path[MAXN];
static unsigned long long targetRank, solutionCount;

/* 从下标 start 开始选数，还需要凑出 remaining */
static int dfs(int start, long long remaining, int depth)
{
    int i;

    if (remaining == 0)
    {
        ++solutionCount;

        if (solutionCount == targetRank)
        {
            answerLength = depth;
            return 1;
        }

        return 0;
    }

    /* 剩余元素全部选择也不够，直接剪枝 */
    if (start == n || suffix[start] < remaining)
        return 0;

    /* 从小到大选择下一个元素，保证字典序 */
    for (i = start; i < n; ++i)
    {
        if (a[i] > remaining)
            break;

        path[depth] = a[i];

        if (dfs(i + 1, remaining - a[i], depth + 1))
            return 1;
    }

    return 0;
}

int main(void)
{
    long long target;
    int i;

    if (scanf("%d%lld", &n, &target) != 2)
        return 0;

    for (i = 0; i < n; ++i)
    {
        if (scanf("%lld", &a[i]) != 1)
            return 0;
    }

    if (scanf("%llu", &targetRank) != 1)
        return 0;

    /* suffix[i] 表示 a[i] 到 a[n-1] 的总和 */
    for (i = n - 1; i >= 0; --i)
        suffix[i] = suffix[i + 1] + a[i];

    if (!dfs(0, target, 0))
    {
        puts("No Solution!");
    }
    else
    {
        for (i = 0; i < answerLength; ++i)
        {
            if (i > 0)
                printf(" ");

            printf("%lld", path[i]);
        }

        printf("\n");
    }

    return 0;
}