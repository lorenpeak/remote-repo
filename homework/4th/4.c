#include <stdio.h>

#define MAXS 200

static int bestServers = 0;
static int bestDifference = MAXS;

/* loads 始终按从大到小排列 */
static void search(const int loads[], int count)
{
    int largest = loads[0];
    int second = count > 1 ? loads[1] : 0;
    int lower = largest / 3 + 1;
    int a;

    /* 同时满足 3*a > largest、2*a > second */
    if (lower <= second / 2)
        lower = second / 2 + 1;

    /* 没有合法拆分，当前状态为终止状态 */
    if (lower > largest / 2)
    {
        int difference = largest - loads[count - 1];

        if (count > bestServers ||
            (count == bestServers && difference < bestDifference))
        {
            bestServers = count;
            bestDifference = difference;
        }

        return;
    }

    for (a = lower; a <= largest / 2; ++a)
    {
        int next[MAXS + 1];
        int i, j;

        /* 删除原来的最大负载 */
        for (i = 1; i < count; ++i)
            next[i - 1] = loads[i];

        /* 加入拆分后的两个负载 */
        next[count - 1] = a;
        next[count] = largest - a;

        /* 将两个新增元素插入，保持降序 */
        for (i = count - 1; i <= count; ++i)
        {
            int value = next[i];

            j = i;
            while (j > 0 && next[j - 1] < value)
            {
                next[j] = next[j - 1];
                --j;
            }

            next[j] = value;
        }

        search(next, count + 1);
    }
}

int main(void)
{
    int s, initial[1];

    if (scanf("%d", &s) != 1)
        return 0;

    initial[0] = s;
    search(initial, 1);

    printf("%d %d\n", bestServers, bestDifference);

    return 0;
}