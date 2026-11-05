#include <stdio.h>
#define MAXN 1000
#define MAXCELLS (MAXN * MAXN)
static int nextA[MAXCELLS], nextB[MAXCELLS], board[MAXCELLS];
static int parts[170], partCount, size;
static char outputBuffer[1 << 20];
static const unsigned char tours[3][3][100] = {
    {
        {18, 7, 3, 11, 22, 35, 27, 31, 20, 24, 32, 28, 17, 4, 15, 23, 34, 26, 30, 19, 6, 2, 10, 21, 13, 0, 8, 12, 25, 33, 29, 16, 5, 9, 1, 14},
        {24, 9, 3, 13, 7, 22, 39, 45, 30, 47, 37, 43, 33, 16, 1, 18, 8, 2, 12, 6, 23, 38, 28, 34, 40, 25, 42, 32, 17, 0, 10, 27, 44, 29, 35, 20, 14, 31, 46, 36, 19, 4, 21, 15, 5, 11, 26, 41},
        {30, 11, 3, 15, 7, 19, 38, 59, 47, 55, 43, 51, 32, 40, 52, 44, 56, 48, 29, 8, 27, 39, 58, 46, 54, 35, 23, 2, 10, 31, 50, 42, 21, 0, 12, 4, 16, 24, 5, 17, 9, 28, 36, 57, 49, 37, 18, 6, 25, 13, 1, 20, 41, 33, 45, 53, 34, 26, 14, 22},
    },
    {
        {18, 7, 3, 11, 22, 35, 46, 38, 42, 31, 44, 36, 25, 12, 1, 14, 6, 2, 10, 23, 27, 40, 29, 33, 41, 45, 37, 24, 20, 16, 5, 9, 17, 4, 15, 28, 32, 43, 39, 47, 34, 21, 13, 0, 8, 19, 30, 26},
        {24, 9, 3, 13, 7, 22, 5, 15, 30, 47, 62, 52, 58, 48, 33, 16, 1, 11, 17, 0, 10, 4, 14, 31, 46, 63, 53, 59, 49, 32, 26, 20, 37, 43, 60, 54, 39, 45, 55, 61, 51, 57, 40, 34, 28, 38, 23, 6, 21, 36, 42, 27, 44, 50, 56, 41, 35, 25, 8, 2, 19, 29, 12, 18},
        {30, 11, 3, 15, 7, 19, 38, 59, 78, 66, 74, 62, 70, 51, 72, 60, 41, 20, 1, 22, 10, 2, 14, 6, 18, 39, 27, 8, 29, 17, 9, 28, 49, 68, 76, 64, 43, 31, 50, 71, 52, 73, 61, 40, 21, 0, 12, 4, 16, 37, 58, 79, 67, 48, 69, 77, 56, 75, 63, 55, 47, 35, 23, 44, 36, 57, 65, 53, 32, 24, 5, 13, 25, 33, 45, 26, 34, 46, 54, 42},
    },
    {
        {18, 7, 3, 11, 22, 35, 46, 59, 51, 55, 42, 31, 44, 48, 56, 52, 41, 33, 29, 40, 53, 57, 49, 36, 25, 12, 1, 14, 6, 2, 10, 23, 27, 38, 30, 19, 15, 4, 17, 9, 5, 16, 8, 0, 13, 21, 34, 47, 58, 45, 32, 24, 20, 28, 39, 43, 54, 50, 37, 26},
        {24, 9, 3, 13, 7, 22, 5, 15, 30, 47, 62, 79, 69, 75, 65, 48, 33, 16, 1, 11, 17, 0, 10, 4, 14, 20, 26, 32, 49, 64, 74, 59, 76, 70, 55, 45, 39, 54, 71, 77, 60, 66, 72, 57, 40, 25, 8, 2, 19, 34, 28, 18, 12, 6, 23, 38, 21, 31, 37, 43, 53, 63, 78, 68, 51, 36, 42, 27, 44, 29, 46, 61, 67, 73, 58, 52, 35, 50, 56, 41},
        {30, 11, 3, 15, 7, 19, 38, 59, 78, 99, 87, 95, 83, 91, 70, 51, 72, 80, 92, 71, 90, 82, 94, 86, 98, 79, 58, 39, 18, 6, 27, 8, 29, 17, 9, 28, 49, 68, 89, 97, 76, 88, 96, 84, 63, 75, 67, 48, 69, 77, 85, 93, 81, 60, 41, 20, 1, 22, 10, 2, 14, 26, 5, 13, 21, 0, 12, 4, 16, 37, 25, 33, 45, 57, 36, 24, 32, 40, 61, 53, 65, 73, 52, 64, 56, 44, 23, 31, 50, 62, 43, 35, 47, 66, 74, 55, 34, 46, 54, 42},
    },
};

static int cell(int r, int c) { return r * size + c; }
static int mapCell(int local, int width, int row, int col)
{
    return cell(row + local / width, col + local % width);
}
static void replaceNeighbor(int v, int old, int replacement)
{
    if (nextA[v] == old)
        nextA[v] = replacement;
    else
        nextB[v] = replacement;
}
static void join(int a, int b, int c, int d)
{
    replaceNeighbor(a, b, c);
    replaceNeighbor(b, a, d);
    replaceNeighbor(c, d, a);
    replaceNeighbor(d, c, b);
}
static void construct(int n, int startRow, int startCol)
{
    int remaining = n, i, j, row = 0, col;
    int previous = -1, current, step;
    size = n;
    partCount = 0;
    while (remaining > 10)
    {
        parts[partCount++] = 6;
        remaining -= 6;
    }
    parts[partCount++] = remaining;

    for (i = 0; i < partCount; ++i)
    {
        col = 0;
        for (j = 0; j < partCount; ++j)
        {
            int h = parts[i], w = parts[j], area = h * w, k;
            const unsigned char *tour = tours[(h - 6) / 2][(w - 6) / 2];
            for (k = 0; k < area; ++k)
            {
                int v = mapCell(tour[k], w, row, col);
                nextA[v] = mapCell(tour[(k + area - 1) % area], w, row, col);
                nextB[v] = mapCell(tour[(k + 1) % area], w, row, col);
            }
            if (j > 0)
            {
                join(cell(row, col - 1), cell(row + 2, col - 2),
                     cell(row + 1, col + 1), cell(row + 3, col));
            }
            col += w;
        }
        row += parts[i];
    }

    row = 0;
    for (i = 1; i < partCount; ++i)
    {
        row += parts[i - 1];
        join(cell(row - 1, 0), cell(row - 2, 2),
             cell(row + 1, 1), cell(row, 3));
    }

    current = cell(startRow, startCol);
    for (step = 1; step <= n * n; ++step)
    {
        int following = nextA[current] == previous ? nextB[current] : nextA[current];
        board[current] = step;
        previous = current;
        current = following;
    }
}
int main(void)
{
    int n, a, b, r, c;
    if (scanf("%d%d%d", &n, &a, &b) != 3)
        return 0;
    if (n & 1)
    {
        puts("No Solution");
        return 0;
    }
    construct(n, a - 1, b - 1);
    setvbuf(stdout, outputBuffer, _IOFBF, sizeof(outputBuffer));
    for (r = 0; r < n; ++r)
    {
        for (c = 0; c < n; ++c)
        {
            if (c)
                putchar(' ');
            printf("%d", board[cell(r, c)]);
        }
        putchar('\n');
    }
    return 0;
}
