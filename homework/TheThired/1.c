#include <stdio.h>

static long long minLL(long long a, long long b)
{
    return a < b ? a : b;
}

/* 求两个区间合并后的第 k 小元素，k 从 1 开始 */
static long long kth(long long low1, long long n1,
                     long long low2, long long n2, long long k)
{
    for (;;)
    {
        long long take1, take2, value1, value2;

        /* 某个区间已空，直接从另一个区间取答案 */
        if (n1 == 0)
            return 2 * (low2 + k - 1);

        if (n2 == 0)
            return 2 * (low1 + k - 1) + 1;

        /* 第 1 小就是两个区间首元素的较小值 */
        if (k == 1)
            return minLL(2 * low1 + 1, 2 * low2);

        /* 各取前 k/2 个，不超过各自剩余长度 */
        take1 = minLL(n1, k / 2);
        take2 = minLL(n2, k / 2);

        value1 = 2 * (low1 + take1 - 1) + 1;
        value2 = 2 * (low2 + take2 - 1);

        if (value1 <= value2)
        {
            /* 排除第一个区间的前 take1 个元素 */
            low1 += take1;
            n1 -= take1;
            k -= take1;
        }
        else
        {
            /* 排除第二个区间的前 take2 个元素 */
            low2 += take2;
            n2 -= take2;
            k -= take2;
        }
    }
}

int main(void)
{
    long long n;
    int t, i;

    if (scanf("%lld%d", &n, &t) != 2)
        return 0;

    for (i = 0; i < t; ++i)
    {
        long long low1, n1, low2, n2, k;

        if (scanf("%lld%lld%lld%lld",
                  &low1, &n1, &low2, &n2) != 4)
            return 0;

        k = (n1 + n2 + 1) / 2;

        printf("%lld\n", kth(low1, n1, low2, n2, k));
    }

    return 0;
}