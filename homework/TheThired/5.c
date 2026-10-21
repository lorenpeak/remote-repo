#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXN 100
#define MAXW 10000

__extension__ typedef unsigned __int128 WideInt;

static int weight[MAXN + 1], value[MAXN + 1];
static WideInt profit[MAXN + 1];
static WideInt dp[MAXW + 1];
static int usedWeight[MAXW + 1];

static unsigned char take[MAXN + 1][MAXW + 1];
static int exact[MAXN + 1], approximate[MAXN + 1];

/* 将 epsilon 转换为 numerator / denominator */
static void parseEpsilon(const char *text, WideInt *numerator,
                         WideInt *denominator)
{
    int fractional = 0;

    *numerator = 0;
    *denominator = 1;

    while (*text && *text != 'e' && *text != 'E')
    {
        if (*text == '.')
        {
            fractional = 1;
        }
        else if (*text >= '0' && *text <= '9')
        {
            *numerator = *numerator * 10 + (*text - '0');
            if (fractional)
                *denominator *= 10;
        }
        ++text;
    }

    /* 同时支持科学计数法 */
    if (*text)
    {
        int exponent = atoi(text + 1);

        while (exponent > 0)
        {
            *numerator *= 10;
            --exponent;
        }
        while (exponent < 0)
        {
            *denominator *= 10;
            ++exponent;
        }
    }

    while (*denominator > 1 && *numerator % 10 == 0)
    {
        *numerator /= 10;
        *denominator /= 10;
    }
}

/* 使用 profit 作为物品价值求解，并还原选择向量 */
static void solve(int n, int capacity, int answer[])
{
    int i, c;

    memset(dp, 0, sizeof(dp));
    memset(usedWeight, 0, sizeof(usedWeight));
    memset(take, 0, sizeof(take));
    memset(answer, 0, (size_t)(n + 1) * sizeof(answer[0]));

    for (i = 1; i <= n; ++i)
    {
        /* 容量倒序遍历，保证每件物品最多选择一次 */
        for (c = capacity; c >= weight[i]; --c)
        {
            WideInt candidate = dp[c - weight[i]] + profit[i];
            int newWeight = usedWeight[c - weight[i]] + weight[i];

            if (candidate > dp[c] ||
                (candidate == dp[c] && newWeight < usedWeight[c]))
            {
                dp[c] = candidate;
                usedWeight[c] = newWeight;
                take[i][c] = 1;
            }
        }
    }

    /* 反向还原选择的物品 */
    c = capacity;
    for (i = n; i >= 1; --i)
    {
        if (take[i][c])
        {
            answer[i] = 1;
            c -= weight[i];
        }
    }
}

static void printAnswer(int n, const int answer[])
{
    int i, total = 0;

    for (i = 1; i <= n; ++i)
        total += answer[i] * value[i];

    printf("%d\n", total);

    for (i = 1; i <= n; ++i)
        printf("%d ", answer[i]);

    printf("\n");
}

int main(void)
{
    int n, capacity, i, maxValue = 0;
    WideInt numerator, denominator;
    char epsilonText[64];

    if (scanf("%d%d%63s", &n, &capacity, epsilonText) != 3)
        return 0;

    for (i = 1; i <= n; ++i)
    {
        if (scanf("%d", &weight[i]) != 1)
            return 0;
    }

    for (i = 1; i <= n; ++i)
    {
        if (scanf("%d", &value[i]) != 1)
            return 0;

        if (value[i] > maxValue)
            maxValue = value[i];

        profit[i] = value[i];
    }

    /* 第一组：使用原始价值求精确解 */
    solve(n, capacity, exact);

    /* 第二组：使用缩放后的整数价值求近似解 */
    parseEpsilon(epsilonText, &numerator, &denominator);

    for (i = 1; i <= n; ++i)
    {
        profit[i] = (WideInt)2 * n * value[i] * denominator / (numerator * maxValue);
    }

    solve(n, capacity, approximate);

    printAnswer(n, exact);
    printAnswer(n, approximate);

    return 0;
}