#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define STATES 362880
#define INF 0x3f3f3f3f

static const int factorial[10] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880};
static int distance[STATES], parent[STATES];
static unsigned char heuristic[STATES];
static uint64_t state[STATES];
static int heap[STATES + 1], position[STATES], heapSize;
static int goalRow[9], goalCol[9];

static int absolute(int x) { return x < 0 ? -x : x; }
static int rankState(const int a[])
{
    int i, j, rank = 0;
    for (i = 0; i < 9; ++i)
    {
        int smaller = 0;
        for (j = i + 1; j < 9; ++j)
            if (a[j] < a[i])
                ++smaller;
        rank += smaller * factorial[8 - i];
    }
    return rank;
}
static uint64_t encode(const int a[])
{
    uint64_t code = 0;
    int i;
    for (i = 0; i < 9; ++i)
        code |= (uint64_t)a[i] << (4 * i);
    return code;
}
static void decode(uint64_t code, int a[])
{
    int i;
    for (i = 0; i < 9; ++i)
        a[i] = (int)((code >> (4 * i)) & 15);
}
static int parity(const int a[])
{
    int i, j, result = 0;
    for (i = 0; i < 9; ++i)
        if (a[i])
            for (j = i + 1; j < 9; ++j)
                if (a[j] && a[i] > a[j])
                    result ^= 1;
    return result;
}
static int less(int a, int b)
{
    int fa = distance[a] + heuristic[a];
    int fb = distance[b] + heuristic[b];
    if (fa != fb)
        return fa < fb;
    if (heuristic[a] != heuristic[b])
        return heuristic[a] < heuristic[b];
    return a < b;
}
static void exchange(int a, int b)
{
    int t = heap[a];
    heap[a] = heap[b];
    heap[b] = t;
    position[heap[a]] = a;
    position[heap[b]] = b;
}
static void pushOrDecrease(int vertex)
{
    int p;
    if (!position[vertex])
    {
        heap[++heapSize] = vertex;
        position[vertex] = heapSize;
    }
    p = position[vertex];
    while (p > 1 && less(heap[p], heap[p / 2]))
    {
        exchange(p, p / 2);
        p /= 2;
    }
}
static int pop(void)
{
    int result = heap[1], p = 1;
    position[result] = 0;
    if (--heapSize == 0)
        return result;
    heap[1] = heap[heapSize + 1];
    position[heap[1]] = 1;
    for (;;)
    {
        int child = p * 2;
        if (child > heapSize)
            break;
        if (child < heapSize && less(heap[child + 1], heap[child]))
            ++child;
        if (!less(heap[child], heap[p]))
            break;
        exchange(p, child);
        p = child;
    }
    return result;
}
static int astar(const int start[], const int goal[])
{
    static const int dr[4] = {-1, 1, 0, 0};
    static const int dc[4] = {0, 0, -1, 1};
    int i, startRank, goalRank, h = 0;
    if (parity(start) != parity(goal))
        return -1;
    for (i = 0; i < 9; ++i)
    {
        goalRow[goal[i]] = i / 3;
        goalCol[goal[i]] = i % 3;
    }
    memset(distance, 0x3f, sizeof(distance));
    memset(position, 0, sizeof(position));
    heapSize = 0;
    startRank = rankState(start);
    goalRank = rankState(goal);
    for (i = 0; i < 9; ++i)
        if (start[i])
            h += absolute(i / 3 - goalRow[start[i]]) + absolute(i % 3 - goalCol[start[i]]);
    distance[startRank] = 0;
    parent[startRank] = -1;
    heuristic[startRank] = (unsigned char)h;
    state[startRank] = encode(start);
    pushOrDecrease(startRank);

    while (heapSize)
    {
        int u = pop(), a[9], zero = 0, direction;
        if (u == goalRank)
            return u;
        decode(state[u], a);
        while (a[zero] != 0)
            ++zero;
        for (direction = 0; direction < 4; ++direction)
        {
            int r = zero / 3 + dr[direction];
            int c = zero % 3 + dc[direction];
            int next, tile, v, newH;
            if (r < 0 || r >= 3 || c < 0 || c >= 3)
                continue;
            next = r * 3 + c;
            tile = a[next];
            a[zero] = tile;
            a[next] = 0;
            v = rankState(a);
            if (distance[u] + 1 < distance[v])
            {
                newH = heuristic[u] - absolute(r - goalRow[tile]) - absolute(c - goalCol[tile]) + absolute(zero / 3 - goalRow[tile]) + absolute(zero % 3 - goalCol[tile]);
                distance[v] = distance[u] + 1;
                heuristic[v] = (unsigned char)newH;
                parent[v] = u;
                state[v] = state[u] ^ ((uint64_t)tile << (4 * zero)) ^ ((uint64_t)tile << (4 * next));
                pushOrDecrease(v);
            }
            a[next] = tile;
            a[zero] = 0;
        }
    }
    return -1;
}
static void printPath(int vertex)
{
    int a[9], r;
    if (parent[vertex] != -1)
        printPath(parent[vertex]);
    decode(state[vertex], a);
    printf("Step %d:\n", distance[vertex]);
    for (r = 0; r < 3; ++r)
        printf("%d %d %d\n", a[3 * r], a[3 * r + 1], a[3 * r + 2]);
}
int main(void)
{
    int start[9], goal[9], i, result;
    for (i = 0; i < 9; ++i)
        if (scanf("%d", &start[i]) != 1)
            return 0;
    for (i = 0; i < 9; ++i)
        if (scanf("%d", &goal[i]) != 1)
            return 0;
    result = astar(start, goal);
    if (result == -1)
        puts("No Solution.");
    else
        printPath(result);
    return 0;
}
